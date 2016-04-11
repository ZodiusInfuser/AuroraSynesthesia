#include "Spectrum.h"
#include "Common.h"
#include <windows.h>
#include "math.h"
#include "float.h"


ResponceBand::ResponceBand(void)
{
	minFreq = 0.0f;
	maxFreq = 0.0f;
}

ResponceBand::ResponceBand(float lowerFreq, float upperFreq)
{
	minFreq = lowerFreq;
	maxFreq = upperFreq;
}

ResponceCollection::ResponceCollection(void)
{
	colour = RGBColour();
	band = ResponceBand();

	level = 1.0f;
	
	leftIntensity = 0.0f;
	rightIntensity = 0.0f;
}


Spectrum::Spectrum(bool &creationSucceeded)
{
	creationSucceeded = false;

	//
	if(FMOD::System_Create(&system_) == FMOD_OK)
	{
		//
		if((system_->setOutput(FMOD_OUTPUTTYPE_AUTODETECT) == FMOD_OK) && (system_->setDriver(DEFAULT_SOUND_DRIVER) == FMOD_OK))
		{
			//
			if(system_->init(NUM_RECORDING_CHANNELS, FMOD_INIT_NORMAL, NULL) == FMOD_OK)
			{
				FMOD::Sound *sound;
				FMOD_CREATESOUNDEXINFO soundInfo;

				ZeroMemory(&soundInfo, sizeof(FMOD_CREATESOUNDEXINFO));
				soundInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
				soundInfo.length = (unsigned int)(sizeof(int) * NUM_RECORDING_SAMPLES * NUM_RECORDING_SECONDS);
				soundInfo.numchannels = NUM_RECORDING_CHANNELS;
				soundInfo.defaultfrequency = NUM_RECORDING_SAMPLES;
				soundInfo.format = FMOD_SOUND_FORMAT_PCM16;

				//
				FMOD_RESULT err = system_->createSound(NULL, FMOD_2D | FMOD_OPENUSER | FMOD_LOOP_NORMAL | FMOD_SOFTWARE, &soundInfo, &sound);
				if(err == FMOD_OK)
				{
					//
					system_->recordStart(SOUND_IDENTIFIER, sound, true);
					system_->playSound(FMOD_CHANNEL_FREE, sound, false, &channel_);

					//
					FMOD::ChannelGroup *channelGroup;
					system_->createChannelGroup("silentChannel", &channelGroup);
					channel_->setChannelGroup(channelGroup);
					channelGroup->setMute(true);

					creationSucceeded = true;
				}
			}
		}
	}

	//Left Initialisation
	leftSpectrum_ = new float[SPECTRUM_WIDTH];
	leftInstantSpectrum_ = new float[SPECTRUM_WIDTH];
	previousLeftSpectrum_ = new float[SPECTRUM_WIDTH];

	ZeroMemory(leftSpectrum_, sizeof(float) * SPECTRUM_WIDTH);
	ZeroMemory(leftInstantSpectrum_, sizeof(float) * SPECTRUM_WIDTH);	
	ZeroMemory(previousLeftSpectrum_, sizeof(float) * SPECTRUM_WIDTH);

	//Right Initialisation
	rightSpectrum_ = new float[SPECTRUM_WIDTH];
	rightInstantSpectrum_ = new float[SPECTRUM_WIDTH];
	previousRightSpectrum_ = new float[SPECTRUM_WIDTH];

	ZeroMemory(rightSpectrum_, sizeof(float) * SPECTRUM_WIDTH);
	ZeroMemory(rightInstantSpectrum_, sizeof(float) * SPECTRUM_WIDTH);	
	ZeroMemory(previousRightSpectrum_, sizeof(float) * SPECTRUM_WIDTH);	


	bandResponceCollections_ = new list<ResponceCollection>();

	spectrumVolume_ = CLAMP(1.0f, MIN_ADAPTIVE_MULTIPLIER, MAX_ADAPTIVE_MULTIPLIER);
	adaptiveVolumeEnabled_ = false;

	peakVolumeHistory_ = NULL;
	modifySpectrumResponce(1.0f);

	pulseThreshold_ = PULSE_THRESHOLD;
	smoothingFactor_ = SMOOTHING_FACTOR;
}

Spectrum::~Spectrum(void)
{
	//Left Destruction
	delete leftSpectrum_;
	delete leftInstantSpectrum_;
	delete previousLeftSpectrum_;

	//Right Destruction
	delete rightSpectrum_;
	delete rightInstantSpectrum_;
	delete previousRightSpectrum_;


	delete bandResponceCollections_;

	system_->release();
}

void Spectrum::processAudio(void)
{
	//Align playback with recording
	unsigned int recordPosition;
	system_->getRecordPosition(SOUND_IDENTIFIER, &recordPosition);
	channel_->setPosition(recordPosition, FMOD_TIMEUNIT_PCM);

	//Left Channel Processing
	float* newLeftSpectrum = previousLeftSpectrum_;		//Discarding the previous spectrum to become the new spectrum
	previousLeftSpectrum_ = leftSpectrum_;				//Last recorded spectrum becomes the previous spectrum
	channel_->getSpectrum(newLeftSpectrum, SPECTRUM_WIDTH, 0, FMOD_DSP_FFT_WINDOW_HANNING);
	leftSpectrum_ = newLeftSpectrum;					//New spectrum becomes the current spectrum

	//Right Channel Processing
	float* newRightSpectrum = previousRightSpectrum_;	//Discarding the previous spectrum to become the new spectrum
	previousRightSpectrum_ = rightSpectrum_;			//Last recorded spectrum becomes the previous spectrum
	channel_->getSpectrum(newRightSpectrum, SPECTRUM_WIDTH, 1, FMOD_DSP_FFT_WINDOW_HANNING);
	rightSpectrum_ = newRightSpectrum;					//New spectrum becomes the current spectrum



	float peakIntensity = 0.0f;

	//Instant Energy and Peak Calculations
	for(int entry = MIN_ENTRY; entry < SPECTRUM_WIDTH; entry++)
	{
		leftInstantSpectrum_[entry] = leftSpectrum_[entry] - previousLeftSpectrum_[entry];

		rightInstantSpectrum_[entry] = rightSpectrum_[entry] - previousRightSpectrum_[entry];

		peakIntensity = max(max(leftSpectrum_[entry], rightSpectrum_[entry]), peakIntensity);
	}

	peakVolumeHistory_[peakVolumePosition_] = peakIntensity;
	peakVolumePosition_ = (peakVolumePosition_ + 1) % peakVolumeLength_;

	if(adaptiveVolumeEnabled_)
	{
		for(unsigned int peak = 0; peak < peakVolumeLength_; peak++)
		{
			peakIntensity = max(peakVolumeHistory_[peak], peakIntensity);
		}

		spectrumVolume_ = ((CLAMP(ADAPTIVE_VOLUME_LEVEL / peakIntensity, MIN_ADAPTIVE_MULTIPLIER, MAX_ADAPTIVE_MULTIPLIER) - spectrumVolume_) / smoothingFactor_) + spectrumVolume_;
	}
}

void Spectrum::setVolume(float volume)
{
	if(!adaptiveVolumeEnabled_)
	{
		spectrumVolume_ = volume;
	}
}

void Spectrum::enableAdaptiveVolume(void)
{
	adaptiveVolumeEnabled_ = true;
}

void Spectrum::disableAdaptiveVolume(void)
{
	adaptiveVolumeEnabled_ = false;
}

void Spectrum::includeColourBand(RGBColour &colour, ResponceBand &band, float level)
{
	ResponceCollection collection;
	collection.colour = colour;
	collection.band = band;
	collection.level = level;
	collection.leftIntensity = 0.0f;
	collection.rightIntensity = 0.0f;

	bandResponceCollections_->push_back(collection);
}

void Spectrum::includeColourBand(HSVColour &colour, ResponceBand &band, float level)
{
	ResponceCollection collection;
	collection.colour = RGBColour(colour.computeRed(), colour.computeGreen(), colour.computeBlue());
	collection.band = band;
	collection.level = level;
	collection.leftIntensity = 0.0f;
	collection.rightIntensity = 0.0f;

	bandResponceCollections_->push_back(collection);
}

void Spectrum::excludeAllColourBands(void)
{
	bandResponceCollections_->clear();
}

void Spectrum::acquireColourResponces(RGBColour &leftColour, RGBColour &rightColour)
{
	leftColour = RGBColour();
	rightColour = RGBColour();

	list<ResponceCollection>::iterator collectionIter = bandResponceCollections_->begin();
	while(collectionIter != bandResponceCollections_->end())
	{
		float leftIntensity = 0.0f;
		float rightIntensity = 0.0f;

		float minHertz = ENTRY_HZ * (MIN_ENTRY);
		float maxHertz = ENTRY_HZ * (MIN_ENTRY + 1);

		for(int entry = MIN_ENTRY; entry < SPECTRUM_WIDTH; entry++)
		{
			float contribution = CLAMP(((*collectionIter).band.maxFreq - minHertz) / (maxHertz - minHertz), 0.0f, 1.0f) *
								 CLAMP(1.0f - (((*collectionIter).band.minFreq - minHertz) / (maxHertz - minHertz)), 0.0f, 1.0f);

			leftIntensity = max(leftSpectrum_[entry] * (*collectionIter).level * contribution, leftIntensity);
			rightIntensity = max(rightSpectrum_[entry] * (*collectionIter).level * contribution, rightIntensity);

			minHertz = maxHertz;
			maxHertz += ENTRY_HZ;
		}

		//Apply a degree of smoothing to the changing colour responces
		leftIntensity = (((leftIntensity * spectrumVolume_) - (*collectionIter).leftIntensity) / smoothingFactor_) + (*collectionIter).leftIntensity;
		rightIntensity = (((rightIntensity * spectrumVolume_) - (*collectionIter).rightIntensity) / smoothingFactor_) + (*collectionIter).rightIntensity;
		
		//Record newly calculated intensity values
		(*collectionIter).leftIntensity = leftIntensity;
		(*collectionIter).rightIntensity = rightIntensity;


		//Introduce the responce bands colour for the left channel, proportional to the calculated intensity
		leftColour.r += leftIntensity * (*collectionIter).colour.r;
		leftColour.g += leftIntensity * (*collectionIter).colour.g;
		leftColour.b += leftIntensity * (*collectionIter).colour.b;

		//Introduce the responce bands colour for the right channel, proportional to the calculated intensity
		rightColour.r += rightIntensity * (*collectionIter).colour.r;
		rightColour.g += rightIntensity * (*collectionIter).colour.g;
		rightColour.b += rightIntensity * (*collectionIter).colour.b;
		

		collectionIter++;
	}

	if(!bandResponceCollections_->empty())
	{
		leftColour.r = LOGSCALE(leftColour.r / bandResponceCollections_->size());
		leftColour.g = LOGSCALE(leftColour.g / bandResponceCollections_->size());
		leftColour.b = LOGSCALE(leftColour.b / bandResponceCollections_->size());

		rightColour.r = LOGSCALE(rightColour.r / bandResponceCollections_->size());
		rightColour.g = LOGSCALE(rightColour.g / bandResponceCollections_->size());
		rightColour.b = LOGSCALE(rightColour.b / bandResponceCollections_->size());
	}


}

void Spectrum::acquireColourResponces(HSVColour &leftColour, HSVColour &rightColour)
{
	leftColour = HSVColour();
	rightColour = HSVColour();

	RGBColour leftRGB;
	RGBColour rightRGB;

	acquireColourResponces(leftRGB, rightRGB);

	leftRGB.computeHSV(leftColour.h, leftColour.s, leftColour.v);
	rightRGB.computeHSV(rightColour.h, rightColour.s, rightColour.v);
}

void Spectrum::acquireColourPulses(list<RGBColour> &colours, list<PositionAngle> &angles)
{
	list<ResponceCollection>::iterator collectionIter = bandResponceCollections_->begin();
	while(collectionIter != bandResponceCollections_->end())
	{
		float leftDifference = 0.0f;
		float rightDifference = 0.0f;

		float minHertz = ENTRY_HZ * (MIN_ENTRY);
		float maxHertz = ENTRY_HZ * (MIN_ENTRY + 1);

		for(int entry = MIN_ENTRY; entry < SPECTRUM_WIDTH; entry++)
		{
			float contribution = CLAMP(((*collectionIter).band.maxFreq - minHertz) / (maxHertz - minHertz), 0.0f, 1.0f) *
								 CLAMP(1.0f - (((*collectionIter).band.minFreq - minHertz) / (maxHertz - minHertz)), 0.0f, 1.0f);

			leftDifference = max(leftInstantSpectrum_[entry] * (*collectionIter).level * contribution, leftDifference);
			rightDifference = max(rightInstantSpectrum_[entry] * (*collectionIter).level * contribution, rightDifference);

			minHertz = maxHertz;
			maxHertz += ENTRY_HZ;
		}

		leftDifference = ((leftDifference * spectrumVolume_) - pulseThreshold_) / (1.0f - pulseThreshold_);
		rightDifference = ((rightDifference * spectrumVolume_) - pulseThreshold_) / (1.0f - pulseThreshold_);

		if((leftDifference > 0.0f) && (rightDifference > 0.0f))
		{
			colours.push_back(RGBColour(LOGSCALE((*collectionIter).colour.r * ((leftDifference + rightDifference) / 2)),
										LOGSCALE((*collectionIter).colour.g * ((leftDifference + rightDifference) / 2)),
										LOGSCALE((*collectionIter).colour.b * ((leftDifference + rightDifference) / 2))));

			angles.push_back(NORTH_ANGLE);
		}

		//Removed this as the effect became confusing to watch

		else if((leftDifference > 0.0f) && (rightDifference <= 0.0f))
		{
			colours.push_back(RGBColour(LOGSCALE((*collectionIter).colour.r * leftDifference),
										LOGSCALE((*collectionIter).colour.g * leftDifference),
										LOGSCALE((*collectionIter).colour.b * leftDifference)));

			angles.push_back(WEST_ANGLE);
		}
		else if((leftDifference <= 0.0f) && (rightDifference > 0.0f))
		{
			colours.push_back(RGBColour(LOGSCALE((*collectionIter).colour.r * rightDifference),
										LOGSCALE((*collectionIter).colour.g * rightDifference),
										LOGSCALE((*collectionIter).colour.b * rightDifference)));

			angles.push_back(EAST_ANGLE);
		}

		collectionIter++;
	}
}

void Spectrum::acquireColourPulses(list<HSVColour> &colours, list<PositionAngle> &angles)
{
	list<RGBColour>* rgbColours = new list<RGBColour>();

	acquireColourPulses(*rgbColours, angles);

	list<RGBColour>::iterator colourIter = rgbColours->begin();
	while(colourIter != rgbColours->end())
	{
		colours.push_back(HSVColour((*colourIter).computeHue(), (*colourIter).computeSaturation(), (*colourIter).computeValue()));

		colourIter++;
	}

	delete rgbColours;
}

void Spectrum::modifySpectrumResponce(float factor)
{
	peakVolumeLength_ = max((unsigned int)(PEAK_HISTORY_LENGTH * factor), 1);
	peakVolumePosition_ = 0;

	delete peakVolumeHistory_;
	peakVolumeHistory_ = new float[peakVolumeLength_];

	ZeroMemory(peakVolumeHistory_, sizeof(float) * peakVolumeLength_);
	for(unsigned int peak = 0; peak < peakVolumeLength_; peak++)
	{
		peakVolumeHistory_[peak] = 1.0f;
	}

	pulseThreshold_ = PULSE_THRESHOLD * ((1.0f + ((1.0f + factor) / 2.0f)) / 2.0f);
	smoothingFactor_ = SMOOTHING_FACTOR * factor;
}