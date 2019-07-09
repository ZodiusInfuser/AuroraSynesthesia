// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include <dwmapi.h>
#include <Shlobj.h>

#include "math.h"

#include "System.h"
#include "Icon.h"
#include "Menu.h"
#include "resource.h"


System* auroraSynesthesia;
Icon* trayIcon;
Menu* trayMenu;


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // sort through and find what code to run for the message given
    switch(message)
    {
        // this message is read when the window is closed
		case WM_DESTROY:
		case WM_QUERYENDSESSION:
			PostQuitMessage(0);
			break;
		case WM_ENDSESSION:
			delete trayMenu;
			delete auroraSynesthesia;
			delete trayIcon;
			break;
		case WM_POWERBROADCAST:
			if(wParam == PBT_APMRESUMEAUTOMATIC)
			{
				auroraSynesthesia->reinitialise();
				
				delete trayMenu;
				
				trayMenu = new Menu(auroraSynesthesia);
				trayMenu->updateOptions();
			}
			break;
		case TRAY_NOTIFY:
			if(lParam == WM_RBUTTONUP)
			{
				POINT cursorPosition;
				GetCursorPos(&cursorPosition);

				SetForegroundWindow(hWnd); 

				trayMenu->updateOptions();	//Update options before showing menu, to make sure that it reflects any internal system changes
				trayMenu->respondToClick(cursorPosition, hWnd);
			}
			break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc(hWnd, message, wParam, lParam);
} 


void parseIni(CSimpleIni &ini, ScreenMode *screen, SpectrumMode *spectrum, IlluminationPreset *illumination, VisualisationPreset *visualisation,
	ModeResponceSetting *responce, ScreenSamplingSetting *sampling, WhiteBalanceSetting *whitebalance,
	LightBrightnessSetting *brightness, LightSensitivitySetting *sensitivity, LightingArrangementSetting *arrangement, IconUpdateSetting *iconUpdate) {
//Modes
	const WCHAR *auroraMode = ini.GetValue(L"Modes", L"Aurora", NULL);
	if (auroraMode) {
		if (!_wcsicmp(auroraMode, L"average")) {
			*screen = AVERAGE_SCREEN;
		} else if (!_wcsicmp(auroraMode, L"illuminate")) {
			*screen = ILLUMINATE_SCREEN;
		} else if (!_wcsicmp(auroraMode, L"disable")) {
			*screen = DISABLE_SCREEN;
		} else {
			*screen = VIBRANT_SCREEN;
		}
	}
	const WCHAR *synesthesiaMode = ini.GetValue(L"Modes", L"Synesthesia", NULL);
	if (synesthesiaMode) {
		if (!_wcsicmp(synesthesiaMode, L"ambient")) {
			*spectrum = AMBIENT_SPECTRUM;
		} else if (!_wcsicmp(synesthesiaMode, L"immersive")) {
			*spectrum = IMMERSIVE_SPECTRUM;
		} else if (!_wcsicmp(synesthesiaMode, L"disable")) {
			*spectrum = DISABLE_SPECTRUM;
		} else {
			*spectrum = REACTIVE_SPECTRUM;
		}
	}
//Presets
	const WCHAR *illuminationPreset = ini.GetValue(L"Presets", L"Illumination", NULL);
	if (illuminationPreset) {
		if (!_wcsicmp(illuminationPreset, L"spectrum")) {
			*illumination = SPECTRUM_ILLUMINATION;
		} else if (!_wcsicmp(illuminationPreset, L"candle")) {
			*illumination = CANDLE_ILLUMINATION;
		} else if (!_wcsicmp(illuminationPreset, L"relax")) {
			*illumination = RELAX_ILLUMINATION;
		} else {
			*illumination = ORIGINAL_ILLUMINATION;
		}
	}
	const WCHAR *visualisationPreset = ini.GetValue(L"Presets", L"Visualisation", NULL);
	if (visualisationPreset) {
		if (!_wcsicmp(visualisationPreset, L"liquid")) {
			*visualisation = LIQUID_VISUALISATION;
		} else if (!_wcsicmp(visualisationPreset, L"energy")) {
			*visualisation = ENERGY_VISUALISATION;
		} else {
			*visualisation = NATURAL_VISUALISATION;
		}
	}
//Settings
	const WCHAR *modeResponse = ini.GetValue(L"Settings", L"ModeResponse", NULL);
	if (modeResponse) {
		if (!_wcsicmp(modeResponse, L"low")) {
			*responce = LOW_RESPONCE;
		} else if (!_wcsicmp(modeResponse, L"high")) {
			*responce = HIGH_RESPONCE;
		} else if (!_wcsicmp(modeResponse, L"realtime")) {
			*responce = REALTIME_RESPONCE;
		} else {
			*responce = STANDARD_RESPONCE;
		}
	}
	const WCHAR *screenSampling = ini.GetValue(L"Settings", L"ScreenSampling", NULL);
	if (screenSampling) {
		if (!_wcsicmp(screenSampling, L"low")) {
			*sampling = LOW_SAMPLING;
		} else if (!_wcsicmp(screenSampling, L"high")) {
			*sampling = HIGH_SAMPLING;
		} else {
			*sampling = STANDARD_SAMPLING;
		}
	}
	const WCHAR *soundSensitivity = ini.GetValue(L"Settings", L"SoundSensitivity", NULL);
	if (soundSensitivity) {
		if (!_wcsicmp(soundSensitivity, L"delta") || !_wcsicmp(soundSensitivity, L"adaptive")) {
			*sensitivity = ADAPTIVE_SENSITIVITY;
		} else {
			double sense = _wtof(soundSensitivity);
			if (sense <= 0.5) {
				*sensitivity = PFIVE_SENSITIVITY;
			} else if (sense < 1.98) {
				*sensitivity = ONE_SENSITIVITY;
			} else if (sense < 3.98) {
				*sensitivity = TWO_SENSITIVITY;
			} else if (sense < 7.98) {
				*sensitivity = FOUR_SENSITIVITY;
			} else if (sense < 15.98) {
				*sensitivity = EIGHT_SENSITIVITY;
			} else if (sense < 31.98) {
				*sensitivity = SIXTEEN_SENSITIVITY;
			} else if (sense < 63.98) {
				*sensitivity = THIRTYTWO_SENSITIVITY;
			} else if (sense < 127.98) {
				*sensitivity = SIXTYFOUR_SENSITIVITY;
			} else if (sense < 255.98) {
				*sensitivity = ONETWOEIGHT_SENSITIVITY;
			} else {
				*sensitivity = TWOFIVESIX_SENSITIVITY;
			}
		}
	}
	const WCHAR *whiteBalance = ini.GetValue(L"Settings", L"WhiteBalance", NULL);
	if (whiteBalance) {
		int wbalance = _wtoi(whiteBalance);
		if (wbalance == 0) {
			*whitebalance = OFF_WHITE_BALANCE;
		} else if (wbalance == 1) {
			*whitebalance = ONE_WHITE_BALANCE;
		} else if (wbalance == 3) {
			*whitebalance = THREE_WHITE_BALANCE;
		} else if (wbalance == 4) {
			*whitebalance = FOUR_WHITE_BALANCE;
		} else if (wbalance == 5) {
			*whitebalance = FIVE_WHITE_BALANCE;
		} else {
			*whitebalance = TWO_WHITE_BALANCE;
		}
	}
	const WCHAR *lightBrightness = ini.GetValue(L"Settings", L"LightBrightness", NULL);
	if (lightBrightness) {
		int lbrightness = _wtoi(lightBrightness);
		if (lbrightness <= 10) {
			*brightness = TEN_BRIGHTNESS;
		} else if (lbrightness <= 20) {
			*brightness = TWENTY_BRIGHTNESS;
		} else if (lbrightness <= 30) {
			*brightness = THIRTY_BRIGHTNESS;
		} else if (lbrightness <= 40) {
			*brightness = FOURTY_BRIGHTNESS;
		} else if (lbrightness <= 50) {
			*brightness = FIFTY_BRIGHTNESS;
		} else if (lbrightness <= 60) {
			*brightness = SIXTY_BRIGHTNESS;
		} else if (lbrightness <= 70) {
			*brightness = SEVENTY_BRIGHTNESS;
		} else if (lbrightness <= 80) {
			*brightness = EIGHTY_BRIGHTNESS;
		} else if (lbrightness <= 90) {
			*brightness = NINETY_BRIGHTNESS;
		} else {
			*brightness = ONEHUNDRED_BRIGHTNESS;
		}
	}
	const WCHAR *lightingArrangement = ini.GetValue(L"Settings", L"LightingArrangement", NULL);
	if (lightingArrangement) {
		if (!_wcsicmp(lightingArrangement, L"surround")) {
			*arrangement = SURROUND_ARRANGEMENT;
		} else {
			*arrangement = STEREO_ARRANGEMENT;
		}
	}
	const bool updateIcon = ini.GetBoolValue(L"Settings", L"UpdateIcon", true);
	*iconUpdate = updateIcon ? ICON_UPDATE_ENABLED : ICON_UPDATE_DISABLED;
}

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
	HANDLE mutexInstance = CreateMutex(NULL, TRUE, L"Aurora Synesthesia");
	
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		MessageBox(NULL, L"A version of Aurora Synesthesia is already running. Press OK to close.\n", L"Program Already Running", MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		// the handle for the window, filled by a function
		HWND hWnd;
		// this struct holds information for the window class
		WNDCLASSEX wc;

		// clear out the window class for use
		ZeroMemory(&wc, sizeof(WNDCLASSEX));

		// fill in the struct with the needed information
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = hInstance;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = CreatePatternBrush(LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1)));
		wc.lpszClassName = L"AuroraSynesthesiaClass";

		// register the window class
		RegisterClassEx(&wc);

		DEVMODE dm;

		ZeroMemory(&dm, sizeof(dm));

		dm.dmSize = sizeof(dm);

		int width = 360;
		int height = 240;

		if(EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm) != NULL)
		{
			width = dm.dmPelsWidth;
			height = dm.dmPelsHeight;
		}


		// create the window and use the result as the handle
		hWnd = CreateWindowEx(NULL,
							  L"AuroraSynesthesiaClass",    // name of the window class
							  L"Aurora Synesthesia",   // title of the window
							  WS_POPUPWINDOW,    // window style
							  (width / 2) - (360 / 2),    // x-position of the window
							  (height / 2) - (240 / 2),    // y-position of the window
							  360,    // width of the window
							  240,    // height of the window
							  NULL,    // we have no parent window, NULL
							  NULL,    // we aren't using menus, NULL
							  hInstance,    // application handle
							  NULL);    // used with multiple windows, NULL


		HWND hSplashWnd = CreateWindowEx(NULL,
										 L"AuroraSynesthesiaClass",
										 L"",
										 WS_POPUPWINDOW,
										 (width / 2) - (360 / 2),    // x-position of the window
										 (height / 2) - (240 / 2),    // y-position of the window
										 360,
										 240,
										 hWnd,
										 NULL,
										 hInstance,
										 NULL);

		

		ShowWindow(hSplashWnd, SW_SHOWNORMAL);


		ScreenMode screenMode = VIBRANT_SCREEN;
		SpectrumMode spectrumMode = REACTIVE_SPECTRUM;
		IlluminationPreset illumination = ORIGINAL_ILLUMINATION;
		VisualisationPreset visualisation = NATURAL_VISUALISATION;
		ModeResponceSetting responce = STANDARD_RESPONCE;
		ScreenSamplingSetting sampling = STANDARD_SAMPLING;
		WhiteBalanceSetting whiteBalance = TWO_WHITE_BALANCE;
		LightBrightnessSetting brightness = ONEHUNDRED_BRIGHTNESS;
		LightSensitivitySetting sensitivity = ADAPTIVE_SENSITIVITY;
		LightingArrangementSetting arrangement = STEREO_ARRANGEMENT;
		IconUpdateSetting iconUpdate = ICON_UPDATE_ENABLED;

		WCHAR path[MAX_PATH], iniPath[MAX_PATH];
		if (SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path) != S_OK) {
			iniPath[0] = 0;
		} else {
			wstring appDir = (wstring(path) + L"\\Aurora Synesthesia\\");
			if (CreateDirectory(appDir.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
				lstrcpyn(iniPath, (appDir + L"settings.ini").c_str(), sizeof(iniPath));
			} else {
				iniPath[0] = 0;
			}
		}
		if (iniPath[0]) {
			CSimpleIni ini(false, true, true);
			SI_Error rc = ini.LoadFile(iniPath);
			if (rc >= 0) {
				parseIni(ini, &screenMode, &spectrumMode, &illumination, &visualisation,
					&responce, &sampling, &whiteBalance, &brightness, &sensitivity, &arrangement, &iconUpdate);
			}
		}

		if(strcmp(lpCmdLine, "-a") == 0)
		{
			screenMode = VIBRANT_SCREEN;
			spectrumMode = DISABLE_SPECTRUM;
		}
		
		if(strcmp(lpCmdLine, "-s") == 0)
		{
			screenMode = DISABLE_SCREEN;
			spectrumMode = IMMERSIVE_SPECTRUM;
		}

		if(strcmp(lpCmdLine, "-i") == 0)
		{
			screenMode = ILLUMINATE_SCREEN;
			spectrumMode = DISABLE_SPECTRUM;
		}

		Sleep(1500);

		trayIcon = new Icon(hInstance, hWnd, L"Aurora Synesthesia");
		auroraSynesthesia = new System(trayIcon, screenMode, spectrumMode, illumination, visualisation,
			responce, sampling, whiteBalance, brightness, sensitivity, arrangement, iconUpdate, iniPath);
		trayMenu = new Menu(auroraSynesthesia);
		trayMenu->updateOptions();

		ShowWindow(hSplashWnd, SW_HIDE);

		auroraSynesthesia->execute();

		delete trayMenu;
		delete auroraSynesthesia;
		delete trayIcon;

		DestroyWindow(hSplashWnd);
		DestroyWindow(hWnd);

		UnregisterClass(L"AuroraSynesthesiaClass", hInstance);
	}

	if(mutexInstance != NULL) 
	{ 
	   ReleaseMutex(mutexInstance); 
	} 

    return 0;
}