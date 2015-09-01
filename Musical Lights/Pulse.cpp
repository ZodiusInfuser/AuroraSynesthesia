#include "Pulse.h"
#include "Common.h"
#include <Windows.h>
#include "math.h"

Pulse::Pulse(RGBColour &colour, PositionAngle angle)
{
	colour_.r = CLAMP(colour.r, 0.0f, 1.0f);
	colour_.g = CLAMP(colour.g, 0.0f, 1.0f);
	colour_.b = CLAMP(colour.b, 0.0f, 1.0f);

	decayFactor_ = STANDARD_DECAY_FACTOR;
	startPosition_ = angle;
	currentAngle_ = 0.0f;
	elapsedTime_ = 0;
}

Pulse::Pulse(HSVColour &colour, PositionAngle angle)
{
	colour_.r = CLAMP(colour.computeRed(), 0.0f, 1.0f);
	colour_.g = CLAMP(colour.computeGreen(), 0.0f, 1.0f);
	colour_.b = CLAMP(colour.computeBlue(), 0.0f, 1.0f);

	decayFactor_ = STANDARD_DECAY_FACTOR;
	startPosition_ = angle;
	currentAngle_ = 0.0f;
	elapsedTime_ = 0;
}

Pulse::Pulse(RGBColour &colour, PositionAngle angle, bool shortDelay)
{
	colour_.r = CLAMP(colour.r, 0.0f, 1.0f);
	colour_.g = CLAMP(colour.g, 0.0f, 1.0f);
	colour_.b = CLAMP(colour.b, 0.0f, 1.0f);

	decayFactor_ = (shortDelay) ? INCREASED_DECAY_FACTOR : STANDARD_DECAY_FACTOR;
	startPosition_ = angle;
	currentAngle_ = 0.0f;
	elapsedTime_ = 0;
}

Pulse::Pulse(HSVColour &colour, PositionAngle angle, bool shortDelay)
{
	colour_.r = CLAMP(colour.computeRed(), 0.0f, 1.0f);
	colour_.g = CLAMP(colour.computeGreen(), 0.0f, 1.0f);
	colour_.b = CLAMP(colour.computeBlue(), 0.0f, 1.0f);

	decayFactor_ = (shortDelay) ? INCREASED_DECAY_FACTOR : STANDARD_DECAY_FACTOR;
	startPosition_ = angle;
	currentAngle_ = 0.0f;
	elapsedTime_ = 0;
}

Pulse::~Pulse(void)
{
	//No Operation
}

bool Pulse::hasDissipated(void)
{
	bool result = false;

	if((elapsedTime_ >= PULSE_LIFETIME_IN_MILLISECONDS) || (colour_.computeValue() <= MIN_LIGHT_INTENSITY))
	{
		result = true;
	}

	return result;
}

void Pulse::colourContribution(RGBColour &colour, PositionAngle angle, float intensity)
{
	float clockwiseAngle = currentAngle_ - ((float)angle - (float)startPosition_);
	while(clockwiseAngle > 180.0f){clockwiseAngle -= 360.0f;}
	while(clockwiseAngle < -180.0f){clockwiseAngle += 360.0f;}

	float antiClockwiseAngle = (0 - currentAngle_) - ((float)angle - (float)startPosition_);
	while(antiClockwiseAngle > 180.0f){antiClockwiseAngle -= 360.0f;}
	while(antiClockwiseAngle < -180.0f){antiClockwiseAngle += 360.0f;}

	float clockwiseContribution = CLAMP(1.0f - (ABS(clockwiseAngle) / CONTRIBUTION_ANGLE), 0.0f, 1.0f);
	float antiClockwiseContribution = CLAMP(1.0f - (ABS(antiClockwiseAngle) / CONTRIBUTION_ANGLE), 0.0f, 1.0f);

	colour.r += colour_.r * CLAMP(clockwiseContribution + antiClockwiseContribution, 0.0f, 1.0f) * intensity;
	colour.g += colour_.g * CLAMP(clockwiseContribution + antiClockwiseContribution, 0.0f, 1.0f) * intensity;
	colour.b += colour_.b * CLAMP(clockwiseContribution + antiClockwiseContribution, 0.0f, 1.0f) * intensity;
}

void Pulse::propagatePulse(unsigned int updateTime)
{
	colour_.r /= pow(decayFactor_, (float)updateTime / REFERENCE_TIME_IN_MILLISECONDS);
	colour_.g /= pow(decayFactor_, (float)updateTime / REFERENCE_TIME_IN_MILLISECONDS);
	colour_.b /= pow(decayFactor_, (float)updateTime / REFERENCE_TIME_IN_MILLISECONDS);

	elapsedTime_ += updateTime;
	currentAngle_ += ANGULAR_SPEED_PER_MILLISECOND * updateTime;
}

