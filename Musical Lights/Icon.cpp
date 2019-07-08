#include "Icon.h"
#include "resource.h"

Icon::Icon(HINSTANCE hInstance, HWND hWnd, LPWSTR tip)
{
	iconImages_ = new HICON[NUM_ICONS];

	iconImages_[0] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON27));
	iconImages_[1] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON26));
	iconImages_[2] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON25));
	iconImages_[3] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON24));
	iconImages_[4] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON23));
	iconImages_[5] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON22));
	iconImages_[6] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON21));
	iconImages_[7] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON20));
	iconImages_[8] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON19));

	iconImages_[9] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON18));
	iconImages_[10] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON17));
	iconImages_[11] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON16));
	iconImages_[12] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON15));
	iconImages_[13] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON14));
	iconImages_[14] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON13));
	iconImages_[15] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON12));
	iconImages_[16] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON11));
	iconImages_[17] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON10));

	iconImages_[18] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON9));
	iconImages_[19] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON8));
	iconImages_[20] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON7));
	iconImages_[21] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON6));
	iconImages_[22] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON5));
	iconImages_[23] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON4));
	iconImages_[24] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON3));
	iconImages_[25] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	iconImages_[26] = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));


	iconData_.cbSize = sizeof(NOTIFYICONDATA);
	iconData_.hWnd = hWnd;
	iconData_.uID = 1;    
	iconData_.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;  
	iconData_.uCallbackMessage = TRAY_NOTIFY;
	iconData_.hIcon = iconImages_[NUM_ICONS - 1];
	wcscpy_s(iconData_.szTip, tip);

	timePassed_ = 0;

	Shell_NotifyIcon(NIM_ADD, &iconData_);
}

Icon::~Icon(void)
{
	Shell_NotifyIcon(NIM_DELETE, &iconData_);

	delete[] iconImages_;
}

void Icon::representColour(RGBColour colour, unsigned int updateTime)
{
	if (!responceToColor_) {
		return;
	}

	if (updateTime <= 0) {
		updateTime = 1;
	}
	updateTime = 1000 / updateTime;

	timePassed_ += updateTime;

	if(timePassed_ >= ICON_DURATION)
	{
		unsigned int red = (colour.r > MIN_THRESHOLD) ? ((colour.r > MAX_THRESHOLD) ? 2 : 1) : 0;
		unsigned int green = (colour.g > MIN_THRESHOLD) ? ((colour.g > MAX_THRESHOLD) ? 2 : 1) : 0;
		unsigned int blue = (colour.b > MIN_THRESHOLD) ? ((colour.b > MAX_THRESHOLD) ? 2 : 1) : 0;

		iconData_.hIcon = iconImages_[(red * 9) + (green * 3) + (blue * 1)];

		Shell_NotifyIcon(NIM_MODIFY, &iconData_);

		timePassed_ = 0;
	}
}

void Icon::representColour(HSVColour colour, unsigned int updateTime)
{
	RGBColour rgb;

	colour.computeRGB(rgb.r, rgb.g, rgb.b);

	representColour(rgb, updateTime);
}

void Icon::changeToolTip(LPWSTR tip)
{
	wcscpy_s(iconData_.szTip, tip);

	Shell_NotifyIcon(NIM_MODIFY, &iconData_);
}

void Icon::responceToColor(bool responce) {
	responceToColor_ = responce;
	if (!responceToColor_) {
		iconData_.hIcon = iconImages_[NUM_ICONS - 1];
		Shell_NotifyIcon(NIM_MODIFY, &iconData_);
	}
}