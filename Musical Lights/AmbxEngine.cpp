#include "AmbxEngine.h"

// include the basic windows header file
#include <windows.h>



AmbxEngine::AmbxEngine(unsigned int fadeTime, bool &creationSucceeded)
{
	creationSucceeded = false;

	//Initialise the amBX interface
	amBXInterface_ = NULL;
	if(amBXCreateInterface(&amBXInterface_, 1, 0, PROGRAM_NAME, PROGRAM_VERSION, NULL, NULL) == amBX_OK)
	{
		//Iterate through the lights array and assign interfaces
		for(unsigned int position = 0; position < NUM_POSITIONS; position++)
		{
			if(amBXInterface_->createLight(amBXInterface_,
				(amBX_loc)(1 << position), (amBX_height)amBX_EVERYHEIGHT, &amBXLights_[position]) == amBX_OK)
			{
				amBXLights_[position]->setUpdateProperties(amBXLights_[position], 10, MIN_LIGHT_INTENSITY);
				amBXLights_[position]->setFadeTime(amBXLights_[position], fadeTime);
			}
		}

		pulseList_ = new list<Pulse>();
		surroundEffectEnabled_ = false;

		lightSaturation_ = DEFAULT_SATURATION;
		lightBrightness_ = DEFAULT_BRIGHTNESS;
		lightBalance_ = DEFAULT_WHITE_BALANCE;
		ZeroMemory(lightColours_, sizeof(RGBColour) * NUM_POSITIONS);

		creationSucceeded = true;
	}
}

AmbxEngine::~AmbxEngine(void)
{
	//Release the amBX interface
	amBXInterface_->release(amBXInterface_);

	delete pulseList_;
}

void AmbxEngine::enableSurroundEffect(void)
{
	surroundEffectEnabled_ = true;
}

void AmbxEngine::disableSurroundEffect(void)
{
	surroundEffectEnabled_ = false;
}

float AmbxEngine::getSaturation(void)
{
	return lightSaturation_;
}

float AmbxEngine::getBrightness(void)
{
	return lightBrightness_;
}

float AmbxEngine::getWhiteBalance(void)
{
	return lightBalance_;
}

void AmbxEngine::setSaturation(float saturation)
{
	lightSaturation_ = CLAMP(saturation, 0.0f, 1.0f);
}

void AmbxEngine::setBrightness(float brightness)
{
	lightBrightness_ = CLAMP(brightness, 0.0f, 1.0f);
}

void AmbxEngine::setWhiteBalance(float balance)
{
	lightBalance_ = CLAMP(balance, 0.0f, 1.0f);
}

void AmbxEngine::setAmbient(RGBColour left, RGBColour right)
{
	leftAmbient_.r = CLAMP(left.r, 0.0f, 1.0f);
	leftAmbient_.g = CLAMP(left.g, 0.0f, 1.0f);
	leftAmbient_.b = CLAMP(left.b, 0.0f, 1.0f);

	rightAmbient_.r = CLAMP(right.r, 0.0f, 1.0f);
	rightAmbient_.g = CLAMP(right.g, 0.0f, 1.0f);
	rightAmbient_.b = CLAMP(right.b, 0.0f, 1.0f);

	centreAmbient_.r = CLAMP((left.r + right.r) / 2.0f, 0.0f, 1.0f);
	centreAmbient_.g = CLAMP((left.g + right.g) / 2.0f, 0.0f, 1.0f);
	centreAmbient_.b = CLAMP((left.b + right.b) / 2.0f, 0.0f, 1.0f);
}

void AmbxEngine::setAmbient(HSVColour left, HSVColour right)
{
	leftAmbient_.r = CLAMP(left.computeRed(), 0.0f, 1.0f);
	leftAmbient_.g = CLAMP(left.computeGreen(), 0.0f, 1.0f);
	leftAmbient_.b = CLAMP(left.computeBlue(), 0.0f, 1.0f);

	rightAmbient_.r = CLAMP(right.computeRed(), 0.0f, 1.0f);
	rightAmbient_.g = CLAMP(right.computeGreen(), 0.0f, 1.0f);
	rightAmbient_.b = CLAMP(right.computeBlue(), 0.0f, 1.0f);

	centreAmbient_.r = CLAMP((left.computeRed() + right.computeRed()) / 2.0f, 0.0f, 1.0f);
	centreAmbient_.g = CLAMP((left.computeGreen() + right.computeGreen()) / 2.0f, 0.0f, 1.0f);
	centreAmbient_.b = CLAMP((left.computeBlue() + right.computeBlue()) / 2.0f, 0.0f, 1.0f);
}

void AmbxEngine::addToAmbient(RGBColour left, RGBColour right)
{
	leftAmbient_.r += CLAMP(left.r, 0.0f, 1.0f);
	leftAmbient_.g += CLAMP(left.g, 0.0f, 1.0f);
	leftAmbient_.b += CLAMP(left.b, 0.0f, 1.0f);

	rightAmbient_.r += CLAMP(right.r, 0.0f, 1.0f);
	rightAmbient_.g += CLAMP(right.g, 0.0f, 1.0f);
	rightAmbient_.b += CLAMP(right.b, 0.0f, 1.0f);

	centreAmbient_.r += CLAMP((left.r + right.r) / 2.0f, 0.0f, 1.0f);
	centreAmbient_.g += CLAMP((left.g + right.g) / 2.0f, 0.0f, 1.0f);
	centreAmbient_.b += CLAMP((left.b + right.b) / 2.0f, 0.0f, 1.0f);
}

void AmbxEngine::addToAmbient(HSVColour left, HSVColour right)
{
	leftAmbient_.r += CLAMP(left.computeRed(), 0.0f, 1.0f);
	leftAmbient_.g += CLAMP(left.computeGreen(), 0.0f, 1.0f);
	leftAmbient_.b += CLAMP(left.computeBlue(), 0.0f, 1.0f);

	rightAmbient_.r += CLAMP(right.computeRed(), 0.0f, 1.0f);
	rightAmbient_.g += CLAMP(right.computeGreen(), 0.0f, 1.0f);
	rightAmbient_.b += CLAMP(right.computeBlue(), 0.0f, 1.0f);

	centreAmbient_.r += CLAMP((left.computeRed() + right.computeRed()) / 2.0f, 0.0f, 1.0f);
	centreAmbient_.g += CLAMP((left.computeGreen() + right.computeGreen()) / 2.0f, 0.0f, 1.0f);
	centreAmbient_.b += CLAMP((left.computeBlue() + right.computeBlue()) / 2.0f, 0.0f, 1.0f);
}

void AmbxEngine::setAmbient(RGBColour left, RGBColour right, RGBColour centre)
{
	leftAmbient_.r = CLAMP(left.r, 0.0f, 1.0f);
	leftAmbient_.g = CLAMP(left.g, 0.0f, 1.0f);
	leftAmbient_.b = CLAMP(left.b, 0.0f, 1.0f);

	rightAmbient_.r = CLAMP(right.r, 0.0f, 1.0f);
	rightAmbient_.g = CLAMP(right.g, 0.0f, 1.0f);
	rightAmbient_.b = CLAMP(right.b, 0.0f, 1.0f);

	centreAmbient_.r = CLAMP(centre.r, 0.0f, 1.0f);
	centreAmbient_.g = CLAMP(centre.g, 0.0f, 1.0f);
	centreAmbient_.b = CLAMP(centre.b, 0.0f, 1.0f);
}

void AmbxEngine::setAmbient(HSVColour left, HSVColour right, HSVColour centre)
{
	leftAmbient_.r = CLAMP(left.computeRed(), 0.0f, 1.0f);
	leftAmbient_.g = CLAMP(left.computeGreen(), 0.0f, 1.0f);
	leftAmbient_.b = CLAMP(left.computeBlue(), 0.0f, 1.0f);

	rightAmbient_.r = CLAMP(right.computeRed(), 0.0f, 1.0f);
	rightAmbient_.g = CLAMP(right.computeGreen(), 0.0f, 1.0f);
	rightAmbient_.b = CLAMP(right.computeBlue(), 0.0f, 1.0f);

	centreAmbient_.r = CLAMP(centre.computeRed(), 0.0f, 1.0f);
	centreAmbient_.g = CLAMP(centre.computeGreen(), 0.0f, 1.0f);
	centreAmbient_.b = CLAMP(centre.computeBlue(), 0.0f, 1.0f);
}

void AmbxEngine::addToAmbient(RGBColour left, RGBColour right, RGBColour centre)
{
	leftAmbient_.r += CLAMP(left.r, 0.0f, 1.0f);
	leftAmbient_.g += CLAMP(left.g, 0.0f, 1.0f);
	leftAmbient_.b += CLAMP(left.b, 0.0f, 1.0f);

	rightAmbient_.r += CLAMP(right.r, 0.0f, 1.0f);
	rightAmbient_.g += CLAMP(right.g, 0.0f, 1.0f);
	rightAmbient_.b += CLAMP(right.b, 0.0f, 1.0f);

	centreAmbient_.r += CLAMP(centre.r, 0.0f, 1.0f);
	centreAmbient_.g += CLAMP(centre.g, 0.0f, 1.0f);
	centreAmbient_.b += CLAMP(centre.b, 0.0f, 1.0f);
}

void AmbxEngine::addToAmbient(HSVColour left, HSVColour right, HSVColour centre)
{
	leftAmbient_.r += CLAMP(left.computeRed(), 0.0f, 1.0f);
	leftAmbient_.g += CLAMP(left.computeGreen(), 0.0f, 1.0f);
	leftAmbient_.b += CLAMP(left.computeBlue(), 0.0f, 1.0f);

	rightAmbient_.r += CLAMP(right.computeRed(), 0.0f, 1.0f);
	rightAmbient_.g += CLAMP(right.computeGreen(), 0.0f, 1.0f);
	rightAmbient_.b += CLAMP(right.computeBlue(), 0.0f, 1.0f);

	centreAmbient_.r += CLAMP(centre.computeRed(), 0.0f, 1.0f);
	centreAmbient_.g += CLAMP(centre.computeGreen(), 0.0f, 1.0f);
	centreAmbient_.b += CLAMP(centre.computeBlue(), 0.0f, 1.0f);
}

void AmbxEngine::addColourPulses(list<RGBColour> &colours, list<PositionAngle> &angles)
{
	list<RGBColour>::iterator colourIter = colours.begin();
	list<PositionAngle>::iterator angleIter = angles.begin();
	while((colourIter != colours.end()) && (angleIter != angles.end()))
	{
		if(((*colourIter).computeValue() > MIN_LIGHT_INTENSITY) && ((*angleIter) != INVALID_ANGLE))
		{
			pulseList_->push_back(Pulse(*colourIter, *angleIter, surroundEffectEnabled_));
		}

		colourIter++;
		angleIter++;
	}

	colours.clear();
	angles.clear();
}

void AmbxEngine::addColourPulses(list<HSVColour> &colours, list<PositionAngle> &angles)
{
	list<HSVColour>::iterator colourIter = colours.begin();
	list<PositionAngle>::iterator angleIter = angles.begin();
	while((colourIter != colours.end()) && (angleIter != angles.end()))
	{
		if(((*colourIter).v > MIN_LIGHT_INTENSITY) && ((*angleIter) != INVALID_ANGLE))
		{
			pulseList_->push_back(Pulse(*colourIter, *angleIter, surroundEffectEnabled_));
		}

		colourIter++;
		angleIter++;
	}

	colours.clear();
	angles.clear();
}


void AmbxEngine::getLightColour(RGBColour &colour, LightPosition position)
{
	colour = lightColours_[position];
}

void AmbxEngine::getLightColour(HSVColour &colour, LightPosition position)
{
	lightColours_[position].computeHSV(colour.h, colour.s, colour.v);
}

void AmbxEngine::changeFadeTime(unsigned int fadeTime)
{
	//Iterate through the lights array and change fade times
	for(unsigned int position = 0; position < NUM_POSITIONS; position++)
	{
		amBXLights_[position]->setFadeTime(amBXLights_[position], fadeTime);
	}
}

void AmbxEngine::updateLighting(unsigned int updateTime)
{
	ZeroMemory(lightColours_, sizeof(RGBColour) * NUM_POSITIONS);

	//
	applyAmbient(1.0f);
	applyPulses(updateTime, 1.0f);

	HSVColour finalColour;
	float finalSaturation;

	RGBColour whiteColour;

	//Apply colours to lights
	for(unsigned int position = 0; position < NUM_POSITIONS; position++)
	{
		//
		lightColours_[position].computeHSV(finalColour.h, finalColour.s, finalColour.v);
		finalSaturation = CLAMP((lightSaturation_ <= 0.5f) ? (finalColour.s * (lightSaturation_ * 2)) : (1 - ((1 - finalColour.s) * ((1 - lightSaturation_) * 2))), 0.0f, 1.0f);
		
		//
		finalColour.s = 1.0f;
		finalColour.v = CLAMP(finalColour.v, 0.0f, 1.0f) * lightBrightness_;
		finalColour.computeRGB(lightColours_[position].r, lightColours_[position].g, lightColours_[position].b);

		//
		whiteColour = RGBColour(finalColour.v, finalColour.v, finalColour.v * (1.0f - lightBalance_));

		//
		lightColours_[position].r = ((whiteColour.r - lightColours_[position].r) * (1.0f - finalSaturation)) + lightColours_[position].r;
		lightColours_[position].g = ((whiteColour.g - lightColours_[position].g) * (1.0f - finalSaturation)) + lightColours_[position].g;
		lightColours_[position].b = ((whiteColour.b - lightColours_[position].b) * (1.0f - finalSaturation)) + lightColours_[position].b;

		//
		amBXLights_[position]->setCol(amBXLights_[position], lightColours_[position].r, lightColours_[position].g, lightColours_[position].b);
	}

	amBXInterface_->update(amBXInterface_, NULL);
}

void AmbxEngine::applyAmbient(float intensity)
{
	if(surroundEffectEnabled_)
	{
		//Modify all lighting positions based upon the current ambient colours
		for(unsigned int position = 0; position < NUM_POSITIONS; position++)
		{
			//Front Left Ambient
			if(position == NORTH_WEST)
			{
				lightColours_[position].r += leftAmbient_.r * intensity;
				lightColours_[position].g += leftAmbient_.g * intensity;
				lightColours_[position].b += leftAmbient_.b * intensity;
			}

			//Front Right Ambient
			if(position == NORTH_EAST)
			{
				lightColours_[position].r += rightAmbient_.r * intensity;
				lightColours_[position].g += rightAmbient_.g * intensity;
				lightColours_[position].b += rightAmbient_.b * intensity;
			}

			//Front Centre Ambient
			if(position == NORTH)
			{
				lightColours_[position].r += centreAmbient_.r * intensity;
				lightColours_[position].g += centreAmbient_.g * intensity;
				lightColours_[position].b += centreAmbient_.b * intensity;
			}

			//Centre Left Ambient
			if(position == WEST)
			{
				lightColours_[position].r += (leftAmbient_.r / 2.0f) * intensity;
				lightColours_[position].g += (leftAmbient_.g / 2.0f) * intensity;
				lightColours_[position].b += (leftAmbient_.b / 2.0f) * intensity;
			}

			//Centre Right Ambient
			if(position == EAST)
			{
				lightColours_[position].r += (rightAmbient_.r / 2.0f) * intensity;
				lightColours_[position].g += (rightAmbient_.g / 2.0f) * intensity;
				lightColours_[position].b += (rightAmbient_.b / 2.0f) * intensity;
			}

			//Centre Centre Ambient
			if(position == CENTRE)
			{
				lightColours_[position].r += (centreAmbient_.r / 2.0f) * intensity;
				lightColours_[position].g += (centreAmbient_.g / 2.0f) * intensity;
				lightColours_[position].b += (centreAmbient_.b / 2.0f) * intensity;
			}
		}
	}
	else
	{
		//Modify all lighting positions based upon the current ambient colours
		for(unsigned int position = 0; position < NUM_POSITIONS; position++)
		{
			//Left Ambient
			if((position == NORTH_WEST) || (position == WEST) || (position == SOUTH_WEST))
			{
				lightColours_[position].r += leftAmbient_.r * intensity;
				lightColours_[position].g += leftAmbient_.g * intensity;
				lightColours_[position].b += leftAmbient_.b * intensity;
			}

			//Right Ambient
			if((position == NORTH_EAST) || (position == EAST) || (position == SOUTH_EAST))
			{
				lightColours_[position].r += rightAmbient_.r * intensity;
				lightColours_[position].g += rightAmbient_.g * intensity;
				lightColours_[position].b += rightAmbient_.b * intensity;
			}

			//Centre Ambient
			if((position == NORTH) || (position == CENTRE) || (position == SOUTH))
			{
				lightColours_[position].r += centreAmbient_.r * intensity;
				lightColours_[position].g += centreAmbient_.g * intensity;
				lightColours_[position].b += centreAmbient_.b * intensity;
			}
		}
	}
}

void AmbxEngine::applyPulses(unsigned int updateTime, float intensity)
{
	bool morePulsesToCheck = true;

	//Continually loop until no more dissipated pulses are found
	while(morePulsesToCheck)
	{
		//If there are no pulses then loop should exit, otherwise...
		if(!pulseList_->empty())
		{
			//Access the head pulse
			Pulse* pulse = &(pulseList_->front());

			//Determine whether the pulse has dissipated and should therefore be removed
			if(pulse->hasDissipated())
			{
				pulseList_->pop_front();
			}
			else
			{
				morePulsesToCheck = false;
			}
		}
		else
		{
			morePulsesToCheck = false;
		}
	}

	//Optain an iterator for the list of pulses
	list<Pulse>::iterator pulseIter = pulseList_->begin();
	while(pulseIter != pulseList_->end())
	{
		//Modify all lighting positions based upon the current pulse colour and intensity
		for(unsigned int position = 0; position < (NUM_POSITIONS - 1); position++)
		{
			(*pulseIter).colourContribution(lightColours_[position], positionToAngle(position), intensity);
		}
		(*pulseIter).propagatePulse(updateTime);

		pulseIter++;
	}
}

PositionAngle AmbxEngine::positionToAngle(unsigned int position)
{
	PositionAngle angle;

	switch(position)
	{
		case NORTH:
			angle = NORTH_ANGLE;
			break;
		case NORTH_EAST:
			angle = NORTH_EAST_ANGLE;
			break;
		case EAST:
			angle = EAST_ANGLE;
			break;
		case SOUTH_EAST:
			angle = SOUTH_EAST_ANGLE;
			break;
		case SOUTH:
			angle = SOUTH_ANGLE;
			break;
		case SOUTH_WEST:
			angle = SOUTH_WEST_ANGLE;
			break;
		case WEST:
			angle = WEST_ANGLE;
			break;
		case NORTH_WEST:
			angle = NORTH_WEST_ANGLE;
			break;
		default:
			angle = INVALID_ANGLE;
			break;
	}

	return angle;
}