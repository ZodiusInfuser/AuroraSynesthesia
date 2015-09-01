#pragma once

#include "AmbxUtil.h"
#include "Colour.h"

//DEFINITIONS

#define ANGULAR_SPEED_PER_MILLISECOND		0.2f
#define PULSE_LIFETIME_IN_MILLISECONDS		900
#define REFERENCE_TIME_IN_MILLISECONDS		100
#define STANDARD_DECAY_FACTOR				1.2f
#define INCREASED_DECAY_FACTOR				1.44f
#define CONTRIBUTION_ANGLE					45.0f



#define ABS(x)		(((x) < 0.0f) ? (0.0f - (x)) : (x))




class Pulse
{
public:
	Pulse(RGBColour &colour, PositionAngle angle);
	Pulse(HSVColour &colour, PositionAngle angle);
	Pulse(RGBColour &colour, PositionAngle angle, bool shortDelay);
	Pulse(HSVColour &colour, PositionAngle angle, bool shortDelay);
	~Pulse(void);

public:
	bool hasDissipated(void);
	void colourContribution(RGBColour &colour, PositionAngle angle, float intensity);
	void propagatePulse(unsigned int updateTime);

private:
	RGBColour colour_;
	PositionAngle startPosition_;

	float decayFactor_;
	float currentAngle_;
	unsigned int elapsedTime_;
};
