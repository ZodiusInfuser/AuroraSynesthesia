#pragma once

#include "Common.h"


//STRUCTURES

struct HSVColour
{
	HSVColour(void);
	HSVColour(float hue, float sat, float val);

	float computeRed(void);
	float computeGreen(void);
	float computeBlue(void);
	void computeRGB(float &red, float &green, float &blue);

	float h;
	float s;
	float v;
};

struct RGBColour
{
	RGBColour(void);
	RGBColour(float red, float green, float blue);

	float computeHue(void);
	float computeSaturation(void);
	float computeValue(void);
	void computeHSV(float &hue, float &saturation, float &value);

	float r;
	float g;
	float b;
};