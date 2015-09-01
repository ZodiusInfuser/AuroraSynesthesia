// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include <dwmapi.h>

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
		auroraSynesthesia = new System(trayIcon, screenMode, spectrumMode, ORIGINAL_ILLUMINATION, NATURAL_VISUALISATION,
			STANDARD_RESPONCE, STANDARD_SAMPLING, TWO_WHITE_BALANCE, ONEHUNDRED_BRIGHTNESS, ADAPTIVE_SENSITIVITY, STEREO_ARRANGEMENT);
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