#include "Colour.h"

HSVColour::HSVColour(void)
{
	h = 0.0f;
	s = 0.0f;
	v = 0.0f;
}

HSVColour::HSVColour(float hue, float sat, float val)
{
	h = hue;
	s = sat;
	v = val;
}

float HSVColour::computeRed(void)
{
	float red = 0.0f;

	//No colour value?
	if(v == 0.0f)
	{
		red = 0.0f;
	}
	else
	{
		//No colour saturation?
		if(s == 0.0f)
		{
			red = v;
		}
		else
		{
			if(h >= 0.0f && h < 60.0f)
			{
				red = v;
			}
			if(h >= 60.0f && h < 120.0f)
			{
				red = v * (1 - (s * ((h - 60.0f) / 60.0f)));
			}
			if(h >= 120.0f && h < 180.0f)
			{
				red = v * (1 - s);
			}
			if(h >= 180.0f && h < 240.0f)
			{
				red = v * (1 - s);
			}
			if(h >= 240.0f && h < 300.0f)
			{
				red = v * (1 - (s * (1 - ((h - 240.0f) / 60.0f))));
			}
			if(h >= 300.0f && h < 360.0f)
			{
				red = v;
			}
		}
	}

	return red;
}

float HSVColour::computeGreen(void)
{
	float green = 0.0f;

	//No colour value?
	if(v == 0.0f)
	{
		green = 0.0f;
	}
	else
	{
		//No colour saturation?
		if(s == 0.0f)
		{
			green = v;
		}
		else
		{
			if(h >= 0.0f && h < 60.0f)
			{
				green = v * (1 - (s * (1 - ((h - 0.0f) / 60.0f))));
			}
			if(h >= 60.0f && h < 120.0f)
			{
				green = v;
			}
			if(h >= 120.0f && h < 180.0f)
			{
				green = v;
			}
			if(h >= 180.0f && h < 240.0f)
			{
				green = v * (1 - (s * ((h - 180.0f) / 60.0f)));
			}
			if(h >= 240.0f && h < 300.0f)
			{
				green = v * (1 - s);
			}
			if(h >= 300.0f && h < 360.0f)
			{
				green = v * (1 - s);
			}
		}
	}

	return green;
}

float HSVColour::computeBlue(void)
{
	float blue = 0.0f;

	//No colour value?
	if(v == 0.0f)
	{
		blue = 0.0f;
	}
	else
	{
		//No colour saturation?
		if(s == 0.0f)
		{
			blue = v;
		}
		else
		{
			if(h >= 0.0f && h < 60.0f)
			{
				blue = v * (1 - s);
			}
			if(h >= 60.0f && h < 120.0f)
			{
				blue = v * (1 - s);
			}
			if(h >= 120.0f && h < 180.0f)
			{
				blue = v * (1 - (s * (1 - ((h - 120.0f) / 60.0f))));
			}
			if(h >= 180.0f && h < 240.0f)
			{
				blue = v;
			}
			if(h >= 240.0f && h < 300.0f)
			{
				blue = v;
			}
			if(h >= 300.0f && h < 360.0f)
			{
				blue = v * (1 - (s * ((h - 300.0f) / 60.0f)));
			}
		}
	}

	return blue;
}

void HSVColour::computeRGB(float &red, float &green, float &blue)
{
	//No colour value?
	if(v == 0.0f)
	{
		red = 0.0f;
		green = 0.0f;
		blue = 0.0f;
	}
	else
	{
		//No colour saturation?
		if(s == 0.0f)
		{
			red = v;
			green = v;
			blue = v;
		}
		else
		{
			if(h >= 0.0f && h < 60.0f)
			{
				red = v;
				green = v * (1 - (s * (1 - ((h - 0.0f) / 60.0f))));
				blue = v * (1 - s);
			}
			if(h >= 60.0f && h < 120.0f)
			{
				red = v * (1 - (s * ((h - 60.0f) / 60.0f)));
				green = v;
				blue = v * (1 - s);
			}
			if(h >= 120.0f && h < 180.0f)
			{
				red = v * (1 - s);
				green = v;
				blue = v * (1 - (s * (1 - ((h - 120.0f) / 60.0f))));
			}
			if(h >= 180.0f && h < 240.0f)
			{
				red = v * (1 - s);
				green = v * (1 - (s * ((h - 180.0f) / 60.0f)));
				blue = v;
			}
			if(h >= 240.0f && h < 300.0f)
			{
				red = v * (1 - (s * (1 - ((h - 240.0f) / 60.0f))));
				green = v * (1 - s);
				blue = v;
			}
			if(h >= 300.0f && h < 360.0f)
			{
				red = v;
				green = v * (1 - s);
				blue = v * (1 - (s * ((h - 300.0f) / 60.0f)));
			}
		}
	}
}



RGBColour::RGBColour(void)
{
	r = 0.0f;
	g = 0.0f;
	b = 0.0f;
};

RGBColour::RGBColour(float red, float green, float blue)
{
	r = red;
	g = green;
	b = blue;
};

float RGBColour::computeHue(void)
{
	float hue = 0.0f;

	float rgbMax = MAX3(r, g, b);
	float rgbMin = MIN3(r, g, b);

	if(rgbMax == 0.0f)
	{
		hue = 0.0f;
	}
	else
	{
		if((rgbMax - rgbMin) == 0.0f)
		{
			hue = 0.0f;
		}
		else
		{
			float red = (r - rgbMin) / (rgbMax - rgbMin);
			float green = (g - rgbMin) / (rgbMax - rgbMin);
			float blue = (b - rgbMin) / (rgbMax - rgbMin);
			float rgbMax = MAX3(red, green, blue);
			float rgbMin = MIN3(red, green, blue);

			if(red == rgbMax)
			{
				hue = 0.0f + (60.0f * (green - blue));
			}
			else if(green == rgbMax)
			{
				hue = 120.0f + (60.0f * (blue - red));
			}
			else /* blue == rgbMax */
			{
				hue = 240.0f + (60.0f * (red - green));
			}

			if(hue < 0.0f)
			{
				hue += 360.0f;
			}		
		}
	}

	return hue;
}

float RGBColour::computeSaturation(void)
{
	float saturation = MAX3(r, g, b) - MIN3(r, g, b);

	return saturation;
}

float RGBColour::computeValue(void)
{
	float value = MAX3(r, g, b);

	return value;
}

void RGBColour::computeHSV(float &hue, float &saturation, float &value)
{
	float rgbMax = MAX3(r, g, b);
	float rgbMin = MIN3(r, g, b);

	if(rgbMax == 0.0f)
	{
		value = 0.0f;
		saturation = 0.0f;
		hue = 0.0f;
	}
	else
	{
		if((rgbMax - rgbMin) == 0.0f)
		{
			value = rgbMax;
			saturation = 0.0f;
			hue = 0.0f;
		}
		else
		{
			value = rgbMax;
			saturation = (rgbMax - rgbMin) / rgbMax;

			float red = (r - rgbMin) / (rgbMax - rgbMin);
			float green = (g - rgbMin) / (rgbMax - rgbMin);
			float blue = (b - rgbMin) / (rgbMax - rgbMin);
			float rgbMax = MAX3(red, green, blue);
			float rgbMin = MIN3(red, green, blue);

			if(red == rgbMax)
			{
				hue = 0.0f + (60.0f * (green - blue));
			}
			else if(green == rgbMax)
			{
				hue = 120.0f + (60.0f * (blue - red));
			}
			else /* blue == rgbMax */
			{
				hue = 240.0f + (60.0f * (red - green));
			}

			if(hue < 0.0f)
			{
				hue += 360.0f;
			}		
		}
	}
}