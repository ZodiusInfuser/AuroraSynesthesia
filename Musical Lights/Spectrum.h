#pragma once

//FMOD INCLUDES
#include <fmod.hpp>


//INCLUDES

#include "AmbxUtil.h"
#include "Common.h"
#include "Colour.h"
#include <list>

using namespace std;


//DEFINITIONS

#define DEFAULT_SOUND_DRIVER		0

#define SOUND_IDENTIFIER			0

#define NUM_RECORDING_SAMPLES		44100
#define	NUM_RECORDING_SECONDS		0.10f
#define NUM_RECORDING_CHANNELS		2


#define SPECTRUM_WIDTH				2048
#define ENTRY_HZ					10.7666f
#define MIN_ENTRY					2

#define SMOOTHING_FACTOR			2.0f
#define PULSE_THRESHOLD				0.10f

#define ADAPTIVE_VOLUME_LEVEL		0.40f
#define MIN_ADAPTIVE_MULTIPLIER		0.5f
#define MAX_ADAPTIVE_MULTIPLIER		256.0f
#define PEAK_HISTORY_LENGTH			20


//STRUCTURES

struct ResponceBand
{
	ResponceBand(void);
	ResponceBand(float lowerFreq, float upperFreq);

	float minFreq;
	float maxFreq;
};

struct ResponceCollection
{
	ResponceCollection(void);

	RGBColour colour;
	ResponceBand band;

	float level;

	float leftIntensity;
	float rightIntensity;
};


class Spectrum
{
public:
	Spectrum(bool &creationSucceeded);
	~Spectrum(void);

public:
	void processAudio(void);

	void setVolume(float volume);
	void enableAdaptiveVolume(void);
	void disableAdaptiveVolume(void);

	void includeColourBand(RGBColour &colour, ResponceBand &band, float level);
	void includeColourBand(HSVColour &colour, ResponceBand &band, float level);
	void excludeAllColourBands(void);

	void acquireColourResponces(RGBColour &leftColour, RGBColour &rightColour);
	void acquireColourResponces(HSVColour &leftColour, HSVColour &rightColour);

	void acquireColourPulses(list<RGBColour> &colours, list<PositionAngle> &angles);
	void acquireColourPulses(list<HSVColour> &colours, list<PositionAngle> &angles);

	void modifySpectrumResponce(float factor);


private:
	FMOD::System* system_;
	FMOD::Channel* channel_;

	float* leftSpectrum_;
	float* leftInstantSpectrum_;
	float* previousLeftSpectrum_;

	float* rightSpectrum_;
	float* rightInstantSpectrum_;
	float* previousRightSpectrum_;

	float spectrumVolume_;
	bool adaptiveVolumeEnabled_;

	float* peakVolumeHistory_;
	unsigned int peakVolumeLength_;
	unsigned int peakVolumePosition_;

	float pulseThreshold_;
	float smoothingFactor_;
	list<ResponceCollection>* bandResponceCollections_;
};
