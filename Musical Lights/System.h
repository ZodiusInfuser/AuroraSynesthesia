#pragma once

//INCLUDES

#include "Screen.h"
#include "Spectrum.h"
#include "AmbxEngine.h"
#include "Icon.h"
#include "SimpleIni.h"


//DEFINITIONS

#define DEFAULT_UPDATE_TIME						STANDARD_UPDATE_TIME
#define DEFAULT_LIGHTING_STATE					LIGHTS_ENABLED

#define DEFAULT_SCREEN_MODE						DISABLE_SCREEN
#define DEFAULT_SPECTRUM_MODE					DISABLE_SPECTRUM

#define DEFAULT_ILLUMINATION_PRESET				ORIGINAL_ILLUMINATION
#define DEFAULT_VISUALISATION_PRESET			NATURAL_VISUALISATION

#define DEFAULT_MODE_RESPONCE_SETTING			STANDARD_RESPONCE
#define DEFAULT_SCREEN_SAMPLING_SETTING			STANDARD_SAMPLING
#define DEFAULT_WHITE_BALANCE_SETTING			OFF_WHITE_BALANCE
#define DEFAULT_LIGHT_BRIGHTNESS_SETTING		ONEHUNDRED_BRIGHTNESS
#define DEFAULT_LIGHT_SENSITIVITY_SETTING		ADAPTIVE_SENSITIVITY
#define DEFAULT_LIGHTING_ARRANGEMENT_SETTING	STEREO_ARRANGEMENT

#define HIGH_UPDATE_TIME			50
#define STANDARD_UPDATE_TIME		100
#define LOW_UPDATE_TIME				200

#define WHITE_BALANCE_SCALING		0.1f
#define SENSITIVITY_MULTIPLIER		2.0f
#define MAX_SCREEN_ATTEMPTS			2


//ENUMERATORS

enum LightingState
{
	LIGHTS_DISABLED				= 0,
	LIGHTS_ENABLED				= 1,
	NUM_LIGHTING_STATES			= 2
};

enum ScreenMode
{
	AVERAGE_SCREEN				= 0,
	VIBRANT_SCREEN				= 1,
	ILLUMINATE_SCREEN			= 2,
	DISABLE_SCREEN				= 3,
	NUM_SCREEN_MODES			= 4
};

enum SpectrumMode
{
	AMBIENT_SPECTRUM			= 0,
	REACTIVE_SPECTRUM			= 1,
	IMMERSIVE_SPECTRUM			= 2,
	DISABLE_SPECTRUM			= 3,
	NUM_SPECTRUM_MODES			= 4
};


enum IlluminationPreset
{
	ORIGINAL_ILLUMINATION		= 0,
	SPECTRUM_ILLUMINATION		= 1,
	CANDLE_ILLUMINATION			= 2,
	RELAX_ILLUMINATION			= 3,
	CUSTOM_ILLUMINATION			= 4,
	NUM_ILLUMINATION_PRESETS	= 5
};

enum VisualisationPreset
{
	NATURAL_VISUALISATION		= 0,
	LIQUID_VISUALISATION		= 1,
	ENERGY_VISUALISATION		= 2,
	CUSTOM_VISUALISATION		= 3,
	NUM_VISUALISATION_PRESETS	= 4
};


enum ModeResponceSetting
{
	LOW_RESPONCE				= 0,
	STANDARD_RESPONCE			= 1,
	HIGH_RESPONCE				= 2,
	NUM_RESPONCE_SETTINGS		= 3
};

enum ScreenSamplingSetting
{
	LOW_SAMPLING				= 0,
	STANDARD_SAMPLING			= 1,
	HIGH_SAMPLING				= 2,
	NUM_SAMPLING_SETTINGS		= 3
};

enum WhiteBalanceSetting
{
	OFF_WHITE_BALANCE			= 0,
	ONE_WHITE_BALANCE			= 1,
	TWO_WHITE_BALANCE			= 2,
	THREE_WHITE_BALANCE			= 3,
	FOUR_WHITE_BALANCE			= 4,
	FIVE_WHITE_BALANCE			= 5,
	NUM_WHITE_BALANCE_SETTINGS	= 6
};

enum LightBrightnessSetting
{
	TEN_BRIGHTNESS				= 0,
	TWENTY_BRIGHTNESS			= 1,
	THIRTY_BRIGHTNESS			= 2,
	FOURTY_BRIGHTNESS			= 3,
	FIFTY_BRIGHTNESS			= 4,
	SIXTY_BRIGHTNESS			= 5,
	SEVENTY_BRIGHTNESS			= 6,
	EIGHTY_BRIGHTNESS			= 7,
	NINETY_BRIGHTNESS			= 8,
	ONEHUNDRED_BRIGHTNESS		= 9,
	NUM_BRIGHTNESS_SETTINGS		= 10
};

enum LightSensitivitySetting
{
	PFIVE_SENSITIVITY			= -1,
	ONE_SENSITIVITY				= 0,
	TWO_SENSITIVITY				= 1,
	FOUR_SENSITIVITY			= 2,
	EIGHT_SENSITIVITY			= 3,
	SIXTEEN_SENSITIVITY			= 4,
	THIRTYTWO_SENSITIVITY		= 5,
	SIXTYFOUR_SENSITIVITY		= 6,
	ONETWOEIGHT_SENSITIVITY		= 7,
	TWOFIVESIX_SENSITIVITY		= 8,
	ADAPTIVE_SENSITIVITY		= 9,
	NUM_SENSITIVITY_SETTINGS	= 10
};

enum LightingArrangementSetting
{
	STEREO_ARRANGEMENT			= 0,
	SURROUND_ARRANGEMENT		= 1,
	NUM_ARRANGEMENT_SETTINGS	= 2
};



class System
{
public:
	System(Icon* icon, ScreenMode screen, SpectrumMode spectrum, IlluminationPreset illumination, VisualisationPreset visualisation,
		ModeResponceSetting responce, ScreenSamplingSetting sampling, WhiteBalanceSetting whiteBalance,
		LightBrightnessSetting brightness, LightSensitivitySetting sensitivity, LightingArrangementSetting arrangement, const WCHAR *inipath);
	~System(void);

public:
	void reinitialise(void);

	bool successfullyInitialised(void);
	bool usingAurora(void);
	bool usingSynesthesia(void);


	void changeLightingState(LightingState state);

	void changeScreenMode(ScreenMode mode);
	void changeSpectrumMode(SpectrumMode mode);

	void changeIlluminationPreset(IlluminationPreset preset);
	void changeVisualisationPreset(VisualisationPreset preset);

	void changeModeResponceSetting(ModeResponceSetting setting);
	void changeScreenSamplingSetting(ScreenSamplingSetting setting);
	void changeWhiteBalanceSetting(WhiteBalanceSetting setting);
	void changeLightBrightnessSetting(LightBrightnessSetting setting);
	void changeLightSensitivitySetting(LightSensitivitySetting setting);
	void changeLightingArrangementSetting(LightingArrangementSetting setting);


	LightingState currentLightingState(void);

	ScreenMode currentScreenMode(void);
	SpectrumMode currentSpectrumMode(void);

	IlluminationPreset currentIlluminationPreset(void);
	VisualisationPreset currentVisualisationPreset(void);

	ModeResponceSetting currentModeResponceSetting(void);
	ScreenSamplingSetting currentScreenSamplingSetting(void);
	WhiteBalanceSetting currentWhiteBalanceSetting(void);
	LightBrightnessSetting currentLightBrightnessSetting(void);
	LightSensitivitySetting currentLightSensitivitySetting(void);
	LightingArrangementSetting currentLightingArrangementSetting(void);

	void execute(void);

	bool saveSettings(void);

private:
	unsigned int updateTime_;

	bool systemInitialised_;
	bool auroraInitialised_;
	bool synesthesiaInitialised_;
	bool amBXDisabled_;

	LightingState lightingState_;

	ScreenMode screenMode_;
	SpectrumMode spectrumMode_;

	IlluminationPreset illuminationPreset_;
	VisualisationPreset visualisationPreset_;

	ModeResponceSetting modeResponceSetting_;
	ScreenSamplingSetting screenSamplingSetting_;
	WhiteBalanceSetting whiteBalanceSetting_;
	LightBrightnessSetting lightBrightnessSetting_;
	LightSensitivitySetting lightSensitivitySetting_;
	LightingArrangementSetting lightingArrangementSetting_;

	Screen::PixelProcessor** processors_;

	Icon* icon_;
	Screen* screen_;
	Spectrum* spectrum_;
	AmbxEngine* amBX_;

	WCHAR iniPath[MAX_PATH];
};