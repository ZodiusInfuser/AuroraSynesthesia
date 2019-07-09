#pragma once

//INCLUDES

#include <windows.h>

#include "Colour.h"


//DEFINITIONS

#define MIN_THRESHOLD	0.25f
#define MAX_THRESHOLD	0.75f

#define NUM_ICONS		27
#define ICON_DURATION	200


//CLASSES

class Icon
{
public:
	Icon(HINSTANCE hInstance, HWND hWnd, LPWSTR tip);
	~Icon(void);

public:
	void representColour(RGBColour colour, unsigned int updateTime);
	void representColour(HSVColour colour, unsigned int updateTime);
	void changeToolTip(LPWSTR tip);
	void responceToColor(bool responce);

private:
	NOTIFYICONDATA iconData_;
	HICON* iconImages_;	

	unsigned int timePassed_;
	bool responceToColor_;
};
