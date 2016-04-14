#include <dwmapi.h>

#include "Screen.h"
#include "Common.h"
#include "math.h"


ScreenRegion::ScreenRegion(void)
{
	minHorisontal = 0.0f;
	maxHorisontal = 1.0f;

	minVertical = 0.0f;
	maxVertical = 1.0f;
}

ScreenRegion::ScreenRegion(float hMinPercent, float hMaxPercent, float vMinPercent, float vMaxPercent)
{
	minHorisontal = hMinPercent;
	maxHorisontal = hMaxPercent;

	minVertical = vMinPercent;
	maxVertical = vMaxPercent;
}

ColourPeriod::ColourPeriod(void)
{
	startColour = RGBColour();
	endColour = RGBColour();
	
	duration = 0;
}

Screen::Screen(bool &creationSucceeded)
{
	//
	screen_ = GetDC(NULL);

	deviceContextInvalid_ = false;

	width_ = GetDeviceCaps(screen_, HORZRES);
	height_ = GetDeviceCaps(screen_, VERTRES);

	//
	pixelsProcessed_ = 0;

	//
	OSVERSIONINFO osVersionInfo;
	ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFO));
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osVersionInfo);
	usingVistaOrLater_ = (osVersionInfo.dwMajorVersion >= 6);

	//
	aeroPresent_ = false;
	if(usingVistaOrLater_)
	{
		DwmIsCompositionEnabled(&aeroPresent_);
	}
	aeroDisabled_ = false;
	canDisableAero_ = false;
	forgetCanDisableAero_ = false;

	//
	changePixelProcessor(NULL);

	colourPeriods_ = new list<ColourPeriod>();
	illuminateEmulationEnabled_ = false;

	horisontalSamples_ = HORISONTAL_SAMPLES;
	verticalSamples_ = VERTICAL_SAMPLES;

	initDIBits();

	creationSucceeded = true;
}

Screen::Screen(PixelProcessor* processor, bool &creationSucceeded)
{
	//
	screen_ = GetDC(NULL);

	deviceContextInvalid_ = false;

	width_ = GetDeviceCaps(screen_, HORZRES);
	height_ = GetDeviceCaps(screen_, VERTRES);

	//
	pixelsProcessed_ = 0;

	//
	OSVERSIONINFO osVersionInfo;
	ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFO));
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osVersionInfo);
	usingVistaOrLater_ = (osVersionInfo.dwMajorVersion >= 6);

	//
	aeroPresent_ = false;
	if(usingVistaOrLater_)
	{
		DwmIsCompositionEnabled(&aeroPresent_);
	}
	aeroDisabled_ = false;
	canDisableAero_ = false;
	forgetCanDisableAero_ = false;

	//
	changePixelProcessor(processor);

	colourPeriods_ = new list<ColourPeriod>();
	illuminateEmulationEnabled_ = false;

	initDIBits();

	creationSucceeded = true;
}

Screen::~Screen(void)
{
	delete colourPeriods_;

	ReleaseDC(NULL, screen_);
	DeleteObject(bitmap_);
	if (bufPixels_)
		delete[] bufPixels_;
	bufPixels_ = NULL;
}

void Screen::enableIlluminateEmulation(void)
{
	illuminateEmulationEnabled_ = true;

	illuminateColour_ = RGBColour();
	currentPeriod_ = colourPeriods_->begin();
	illuminateTime_ = 0;
}

void Screen::disableIlluminateEmulation(void)
{
	illuminateEmulationEnabled_ = false;
}

void Screen::advanceIlluminateEmulation(unsigned int updateTime)
{
	if(illuminateEmulationEnabled_ && (!colourPeriods_->empty()))
	{
		while(illuminateTime_ >= (*currentPeriod_).duration)
		{
			illuminateTime_ -= (*currentPeriod_).duration;

			currentPeriod_++;

			if(currentPeriod_ == colourPeriods_->end())
			{
				currentPeriod_ = colourPeriods_->begin();
			}
		}

		float percentage = 1.0f - ((float)illuminateTime_ / (float)(*currentPeriod_).duration);

		illuminateColour_.r = (((*currentPeriod_).startColour.r - (*currentPeriod_).endColour.r) * percentage) + (*currentPeriod_).endColour.r;
		illuminateColour_.g = (((*currentPeriod_).startColour.g - (*currentPeriod_).endColour.g) * percentage) + (*currentPeriod_).endColour.g;
		illuminateColour_.b = (((*currentPeriod_).startColour.b - (*currentPeriod_).endColour.b) * percentage) + (*currentPeriod_).endColour.b;

		illuminateTime_ += updateTime;
	}
}

void Screen::includeColourPeriod(RGBColour &fadeFromColour, RGBColour &fadeToColour, unsigned int durationBefore, unsigned int fadeTime, unsigned int durationAfter)
{
	if(!illuminateEmulationEnabled_)
	{
		if(durationBefore > 0)
		{
			ColourPeriod beforePeriod;

			beforePeriod.startColour = RGBColour(fadeFromColour.r, fadeFromColour.g, fadeFromColour.b);
			beforePeriod.endColour = RGBColour(fadeFromColour.r, fadeFromColour.g, fadeFromColour.b);
			beforePeriod.duration = durationBefore;

			colourPeriods_->push_back(beforePeriod);
		}

		if(fadeTime > 0)
		{
			ColourPeriod fadePeriod;

			fadePeriod.startColour = RGBColour(fadeFromColour.r, fadeFromColour.g, fadeFromColour.b);
			fadePeriod.endColour = RGBColour(fadeToColour.r, fadeToColour.g, fadeToColour.b);
			fadePeriod.duration = fadeTime;

			colourPeriods_->push_back(fadePeriod);
		}

		if(durationAfter > 0)
		{
			ColourPeriod afterPeriod;

			afterPeriod.startColour = RGBColour(fadeToColour.r, fadeToColour.g, fadeToColour.b);
			afterPeriod.endColour = RGBColour(fadeToColour.r, fadeToColour.g, fadeToColour.b);
			afterPeriod.duration = durationAfter;

			colourPeriods_->push_back(afterPeriod);
		}
	}
}

void Screen::includeColourPeriod(HSVColour &fadeFromColour, HSVColour &fadeToColour, unsigned int durationBefore, unsigned int fadeTime, unsigned int durationAfter)
{
	includeColourPeriod(RGBColour(fadeFromColour.computeRed(), fadeFromColour.computeGreen(), fadeFromColour.computeBlue()),
						RGBColour(fadeToColour.computeRed(), fadeToColour.computeGreen(), fadeToColour.computeBlue()),
						durationBefore, fadeTime, durationAfter);
}

void Screen::excludeAllColourPeriods(void)
{
	if(!illuminateEmulationEnabled_)
	{
		colourPeriods_->clear();
	}
}

void Screen::acquireScreenRegion(RGBColour &colour, ScreenRegion &region) {
	colour = RGBColour();

	if(!illuminateEmulationEnabled_) {
		//Check whether there is a processor present to perform screen analysis, and
		//that Vista/Win7 Aero is either disabled or not present on the users system
		//aero support
		if((processor_ != NULL)) {
			pixelsProcessed_ = 0;

			unsigned int minXPos = (unsigned int)(width_ * region.minHorisontal);
			unsigned int maxXPos = (unsigned int)(width_ * region.maxHorisontal);

			unsigned int minYPos = (unsigned int)(height_ * region.minVertical);
			unsigned int maxYPos = (unsigned int)(height_ * region.maxVertical);

			unsigned int xSeperation = width_ / horisontalSamples_;
			unsigned int ySeperation = height_ / verticalSamples_;

			//
			if ((!aeroPresent_ || aeroDisabled_)) {
				for (unsigned int y = minYPos + (ySeperation / 2); y < maxYPos; y += ySeperation) {
					for (unsigned int x = minXPos + (xSeperation / 2); x < maxXPos; x += xSeperation) {
						pixelColour_ = GetPixel(screen_, x, y);

						if (pixelColour_ != CLR_INVALID) {
							rgbPixel_ = RGBColour((float)GetRValue(pixelColour_) / 255, (float)GetGValue(pixelColour_) / 255, (float)GetBValue(pixelColour_) / 255);
							//SetPixel(screen_, x, y, RGB(255 - GetRValue(pixelColour_), 255 - GetGValue(pixelColour_), 255 - GetBValue(pixelColour_)));

							processor_->processPixel(colour, rgbPixel_);

							pixelsProcessed_++;
						} else {
							deviceContextInvalid_ = true;
						}
					}
				}
			} else {
				if (tookScreen_) {
					for (unsigned int y = minYPos + (ySeperation / 2); y < maxYPos; y += ySeperation) {
						for (unsigned int x = minXPos + (xSeperation / 2); x < maxXPos; x += xSeperation) {
							BYTE *pixelColour_ = bufPixels_ + 4 * x + y * 4 * width_;

							rgbPixel_ = RGBColour(pixelColour_[2] / 255.0f, pixelColour_[1] / 255.0f, pixelColour_[0] / 255.0f);
							processor_->processPixel(colour, rgbPixel_);
							pixelsProcessed_++;
						}
					}
				} else {
					deviceContextInvalid_ = true;
				}
			}

			if(!deviceContextInvalid_) {
				colour.r /= pixelsProcessed_;
				colour.g /= pixelsProcessed_;
				colour.b /= pixelsProcessed_;
			}
		}
	} else {
		colour.r = illuminateColour_.r;
		colour.g = illuminateColour_.g;
		colour.b = illuminateColour_.b;
	}
}

void Screen::acquireScreenRegion(HSVColour &colour, ScreenRegion &region)
{
	colour = HSVColour();

	RGBColour rgb;

	acquireScreenRegion(rgb, region);

	rgb.computeHSV(colour.h, colour.s, colour.v);
}

void Screen::takeAeroScreen(void) {
	tookScreen_ = false;
	if ((!aeroPresent_ || aeroDisabled_)) {
	} else {
		// get the actual bitmap buffer
		if (0 != GetDIBits(screen_, bitmap_, 0, bitmapInfo_.bmiHeader.biHeight, (LPVOID)bufPixels_, &bitmapInfo_, DIB_RGB_COLORS)) {
			tookScreen_ = true;
		}
	}
}

bool Screen::desktopCompositionPresent(void)
{
	BOOL aeroStatus;
	if(usingVistaOrLater_)
	{
		DwmIsCompositionEnabled(&aeroStatus);
		if(aeroStatus == TRUE)
		{
			aeroPresent_ = true;
			aeroDisabled_ = false;
		}
		else if(!aeroDisabled_)
		{
			aeroPresent_ = false;
		}
	}

	return ((processor_ != NULL) && (aeroPresent_ == TRUE) && !aeroDisabled_);
}


LRESULT CALLBACK ChangeMessageBox(int iCode, WPARAM wParam, LPARAM lParam)
{
	if(iCode == HCBT_ACTIVATE)
	{
		HWND hHWND = (HWND)wParam;
		SetDlgItemText(hHWND, IDYES, L"Yes");
		SetDlgItemText(hHWND, IDNO, L"This Once");
		SetDlgItemText(hHWND, IDCANCEL, L"No");
	}
	 
	return NULL;
}

bool Screen::changePixelProcessor(PixelProcessor* processor) {
	if(aeroPresent_) {
		if(processor != NULL) {
			//aero support
			processor_ = processor;
			aeroDisabled_ = false;
			//Determine whether the user wants to disable Aero in order to experience Aurora
/*			if (!canDisableAero_) {
				//Hook into and create a message box with renamed button text
				HHOOK msgBoxHook = SetWindowsHookEx(WH_CBT, &ChangeMessageBox, 0, GetCurrentThreadId());
				unsigned int messageBoxResult = MessageBox(NULL, L"Average and Vibrant Aurora modes have a known compatability issue with Aero Glass (Desktop Composition) in Windows Vista and 7.\n\nWould you like Aero Glass to be disabled now and all future times Aurora is in use?", L"Destop Composition Detected", MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_DEFBUTTON1);
				UnhookWindowsHookEx(msgBoxHook);

				//Based on the button selected, determine what operation should be performed
				canDisableAero_ = ((messageBoxResult == IDYES) || (messageBoxResult == IDNO));
				forgetCanDisableAero_ = (messageBoxResult == IDNO);
			}

			if (canDisableAero_) {
				DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);
				processor_ = processor;
				aeroDisabled_ = true;
			} else {
				processor_ = NULL;
				aeroDisabled_ = false;
			}*/
		} else {
			DwmEnableComposition(DWM_EC_ENABLECOMPOSITION);
			processor_ = NULL;
			aeroDisabled_ = false;

			//Did the user wish to have Aurora enabled temporarily?
			if(forgetCanDisableAero_) {
				canDisableAero_ = false;
				forgetCanDisableAero_ = false;
			}
		}
	} else {
		processor_ = processor;
	}

	//Has the change in processor taken effect
	return (processor_ == processor);
}

bool Screen::invalidScreen()
{
	return deviceContextInvalid_;
}

void Screen::reinitialiseScreen(void)
{
	ReleaseDC(NULL, screen_);

	screen_ = GetWindowDC(NULL);

	deviceContextInvalid_ = false;

	width_ = GetDeviceCaps(screen_, HORZRES);
	height_ = GetDeviceCaps(screen_, VERTRES);

	//aero support
	DeleteObject(bitmap_);
	if (bufPixels_)
		delete[] bufPixels_;
	bufPixels_ = NULL;
	initDIBits();
}

void Screen::modifySampleSize(float factor)
{
	horisontalSamples_ = (unsigned int)(HORISONTAL_SAMPLES * factor);
	verticalSamples_ = (unsigned int)(VERTICAL_SAMPLES * factor);
}

//aero support
void Screen::initDIBits(void) {
	// Create compatible DC, create a compatible bitmap and copy the screen using BitBlt()
	HDC hCaptureDC = CreateCompatibleDC(screen_);
	bitmap_ = CreateCompatibleBitmap(screen_, width_, height_);
	HGDIOBJ hOld = SelectObject(hCaptureDC, bitmap_);
	BOOL bOK = BitBlt(hCaptureDC, 0, 0, width_, height_, screen_, 0, 0, SRCCOPY | CAPTUREBLT);

	SelectObject(hCaptureDC, hOld); // always select the previously selected object once done
	DeleteDC(hCaptureDC);

	bitmapInfo_ = { 0 };
	bitmapInfo_.bmiHeader.biSize = sizeof(bitmapInfo_.bmiHeader);

	// Get the BITMAPINFO structure from the bitmap
	if (0 == GetDIBits(screen_, bitmap_, 0, 0, NULL, &bitmapInfo_, DIB_RGB_COLORS)) {
		aeroPresent_ = false;
		return;
	}

	// create the bitmap buffer
	bufPixels_ = new BYTE[bitmapInfo_.bmiHeader.biSizeImage];

	// Better do this here - the original bitmap might have BI_BITFILEDS, which makes it
	// necessary to read the color table - you might not want this.
	bitmapInfo_.bmiHeader.biCompression = BI_RGB;

}



AverageProcessor::AverageProcessor(void)
{
}

AverageProcessor::~AverageProcessor(void)
{
}

void AverageProcessor::processPixel(RGBColour &incrementalColour, RGBColour pixelColour)
{
	incrementalColour.r += pixelColour.r;
	incrementalColour.g += pixelColour.g;
	incrementalColour.b += pixelColour.b;
}



VibrantProcessor::VibrantProcessor(void)
{
}

VibrantProcessor::~VibrantProcessor(void)
{
}

void VibrantProcessor::processPixel(RGBColour &incrementalColour, RGBColour pixelColour)
{
	float rgbMin = MIN3(pixelColour.r, pixelColour.g, pixelColour.b);
	float rgbMax = MAX3(pixelColour.r, pixelColour.g, pixelColour.b);

	if(pixelColour.computeSaturation() > 0.0f)
	{
		incrementalColour.r += ((((pixelColour.r - rgbMin) / (rgbMax - rgbMin)) - pixelColour.r) * pixelColour.computeSaturation()) + pixelColour.r;
		incrementalColour.g += ((((pixelColour.g - rgbMin) / (rgbMax - rgbMin)) - pixelColour.g) * pixelColour.computeSaturation()) + pixelColour.g;
		incrementalColour.b += ((((pixelColour.b - rgbMin) / (rgbMax - rgbMin)) - pixelColour.b) * pixelColour.computeSaturation()) + pixelColour.b;
	}
	else
	{
		incrementalColour.r += pixelColour.r;
		incrementalColour.g += pixelColour.g;
		incrementalColour.b += pixelColour.b;
	}
}