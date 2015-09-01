#pragma once

//INCLUDES

#include <windows.h>
#include "Colour.h"
#include <list>

using namespace std;

//DEFINITIONS

#define HORISONTAL_SAMPLES		70
#define VERTICAL_SAMPLES		50


//STRUCTURES

struct ScreenRegion
{
	ScreenRegion(void);
	ScreenRegion(float hMinPercent, float hMaxPercent, float vMinPercent, float vMaxPercent);

	float minHorisontal;
	float maxHorisontal;

	float minVertical;
	float maxVertical;
};

struct ColourPeriod
{
	ColourPeriod(void);

	RGBColour startColour;
	RGBColour endColour;

	unsigned int duration;
};


//CLASSES

class Screen
{
public:
	class PixelProcessor
	{
		public:
			virtual ~PixelProcessor(void){}

		public:
			virtual void processPixel(RGBColour &incrementalColour, RGBColour pixelColour){}
	};

public:
	Screen(bool &creationSucceeded);
	Screen(PixelProcessor* processor, bool &creationSucceeded);
	~Screen(void);


public:
	void enableIlluminateEmulation(void);
	void disableIlluminateEmulation(void);
	void advanceIlluminateEmulation(unsigned int updateTime);

	void includeColourPeriod(RGBColour &fadeFromColour, RGBColour &fadeToColour, unsigned int durationBefore, unsigned int fadeTime, unsigned int durationAfter);
	void includeColourPeriod(HSVColour &fadeFromColour, HSVColour &fadeToColour, unsigned int durationBefore, unsigned int fadeTime, unsigned int durationAfter);
	void excludeAllColourPeriods(void);

	void acquireScreenRegion(RGBColour &colour, ScreenRegion &region);
	void acquireScreenRegion(HSVColour &colour, ScreenRegion &region);

	bool desktopCompositionPresent(void);
	bool changePixelProcessor(PixelProcessor* processor);

	bool invalidScreen(void);
	void reinitialiseScreen(void);

	void modifySampleSize(float factor);

private:
	HDC screen_;
	bool deviceContextInvalid_;
	PixelProcessor* processor_;

	RGBColour illuminateColour_;
	bool illuminateEmulationEnabled_;
	unsigned int illuminateTime_;

	BOOL aeroPresent_;	//Have to use BOOL rather than bool because of the method calls it relates to
	bool aeroDisabled_;
	bool canDisableAero_;
	bool forgetCanDisableAero_;
	bool usingVistaOrLater_;

	unsigned int width_;
	unsigned int height_;
	unsigned int horisontalSamples_;
	unsigned int verticalSamples_;

	unsigned int pixelsProcessed_;
	COLORREF pixelColour_;
	RGBColour rgbPixel_;

	list<ColourPeriod>* colourPeriods_;
	list<ColourPeriod>::iterator currentPeriod_;
};

class AverageProcessor : public Screen::PixelProcessor
{
public:
	AverageProcessor(void);
	~AverageProcessor(void);

public:
	void processPixel(RGBColour &incrementalColour, RGBColour pixelColour);
};

class VibrantProcessor : public Screen::PixelProcessor
{
public:
	VibrantProcessor(void);
	~VibrantProcessor(void);

public:
	void processPixel(RGBColour &incrementalColour, RGBColour pixelColour);
};
