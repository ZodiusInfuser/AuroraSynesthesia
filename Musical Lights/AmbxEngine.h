#pragma once

//INCLUDES

#include "IamBX.h"
#include "IamBX_Light.h"

#include "AmbxUtil.h"
#include "Common.h"
#include "Colour.h"
#include "Pulse.h"

#include <list>

using namespace std;

//DEFINITIONS

#define DEFAULT_SATURATION		0.5f
#define SYNESTHESIA_SATURATION	0.875f
#define HYBRID_SATURATION		0.75f

#define DEFAULT_BRIGHTNESS		1.0f
#define DEFAULT_WHITE_BALANCE	0.0f




class AmbxEngine
{
public:
	AmbxEngine(unsigned int fadeTime, bool &creationSucceeded);
	~AmbxEngine(void);

public:
	void enableSurroundEffect(void);
	void disableSurroundEffect(void);

	float getSaturation(void);
	float getBrightness(void);
	float getWhiteBalance(void);

	void setSaturation(float saturation);
	void setBrightness(float brightness);
	void setWhiteBalance(float balance);

	void setAmbient(RGBColour left, RGBColour right);
	void setAmbient(HSVColour left, HSVColour right);

	void addToAmbient(RGBColour left, RGBColour right);
	void addToAmbient(HSVColour left, HSVColour right);

	void setAmbient(RGBColour left, RGBColour right, RGBColour centre);
	void setAmbient(HSVColour left, HSVColour right, HSVColour centre);

	void addToAmbient(RGBColour left, RGBColour right, RGBColour centre);
	void addToAmbient(HSVColour left, HSVColour right, HSVColour centre);

	void addColourPulses(list<RGBColour> &colours, list<PositionAngle> &angles);
	void addColourPulses(list<HSVColour> &colours, list<PositionAngle> &angles);

	void getLightColour(RGBColour &colour, LightPosition position);
	void getLightColour(HSVColour &colour, LightPosition position);

	void changeFadeTime(unsigned int fadeTime);

	void updateLighting(unsigned int updateTime);

private:
	void applyAmbient(float intensity);
	void applyPulses(unsigned int updateTime, float intensity);
	PositionAngle positionToAngle(unsigned int position);

private:
	IamBX* amBXInterface_;
	IamBX_Light* amBXLights_[NUM_POSITIONS];

	RGBColour leftAmbient_;
	RGBColour rightAmbient_;
	RGBColour centreAmbient_;
	list<Pulse>* pulseList_;
	bool surroundEffectEnabled_;

	float lightSaturation_;
	float lightBrightness_;
	float lightBalance_;
	RGBColour lightColours_[NUM_POSITIONS];
};
