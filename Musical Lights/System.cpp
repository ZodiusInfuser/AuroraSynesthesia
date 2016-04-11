#include "System.h"
#include "math.h"

System::System(Icon* icon, ScreenMode screen, SpectrumMode spectrum, IlluminationPreset illumination, VisualisationPreset visualisation,
	ModeResponceSetting responce, ScreenSamplingSetting sampling, WhiteBalanceSetting whiteBalance,
	LightBrightnessSetting brightness, LightSensitivitySetting sensitivity, LightingArrangementSetting arrangement, const WCHAR *inipath)
{
	lstrcpy(iniPath, inipath);

	icon_ = icon;

	updateTime_ = DEFAULT_UPDATE_TIME;
	
	//
	lightingState_ = DEFAULT_LIGHTING_STATE;

	screenMode_ = DEFAULT_SCREEN_MODE;
	spectrumMode_ = DEFAULT_SPECTRUM_MODE;

	illuminationPreset_ = DEFAULT_ILLUMINATION_PRESET;
	visualisationPreset_ = DEFAULT_VISUALISATION_PRESET;

	modeResponceSetting_ = DEFAULT_MODE_RESPONCE_SETTING;
	screenSamplingSetting_ = DEFAULT_SCREEN_SAMPLING_SETTING;
	whiteBalanceSetting_ = DEFAULT_WHITE_BALANCE_SETTING;
	lightBrightnessSetting_ = DEFAULT_LIGHT_BRIGHTNESS_SETTING;
	lightSensitivitySetting_ = DEFAULT_LIGHT_SENSITIVITY_SETTING;
	lightingArrangementSetting_ = DEFAULT_LIGHTING_ARRANGEMENT_SETTING;

	//
	processors_ = new Screen::PixelProcessor*[NUM_SCREEN_MODES];
	processors_[AVERAGE_SCREEN] = new AverageProcessor();
	processors_[VIBRANT_SCREEN] = new VibrantProcessor();
	processors_[ILLUMINATE_SCREEN] = NULL;
	processors_[DISABLE_SCREEN] = NULL;

	//
	amBX_ = new AmbxEngine(updateTime_, systemInitialised_);
	amBXDisabled_ = !systemInitialised_;

	if(!systemInitialised_)
	{
		MessageBox(NULL, L"amBX could not be detected on your computer. Make sure the amBX service is operating before restarting this program. Press OK to close.\n", L"amBX Initialisation Failed", MB_OK | MB_ICONEXCLAMATION);

		screen_ = NULL;
		spectrum_ = NULL;
	}
	else
	{
		screen_ = new Screen(auroraInitialised_);
		spectrum_ = new Spectrum(synesthesiaInitialised_);

		if(!auroraInitialised_ && !synesthesiaInitialised_)
		{
			systemInitialised_ = false;

			MessageBox(NULL, L"Unable to initialise Aurora and Synesthesia based modes. The program is therefore unable to operate and will exit. Press OK to close.\n", L"Mode Initialisation Failed", MB_OK | MB_ICONEXCLAMATION);
		}
		else
		{
			if(!auroraInitialised_)
			{
				MessageBox(NULL, L"Unableto initialise Aurora based modes. The program can continue to operate but no Screen-to-Light effects will be present.\n", L"Mode Initialisation Failed", MB_OK | MB_ICONEXCLAMATION);
				changeScreenMode(DISABLE_SCREEN);
			}
			else
			{
				changeScreenMode(screen);
			}
			
			if(!synesthesiaInitialised_)
			{
				MessageBox(NULL, L"Unable to initialise Synesthesia based modes. The program can continue to operate but no Sound-to-Light effects will be present.\n", L"Mode Initialisation Failed", MB_OK | MB_ICONEXCLAMATION);
				changeSpectrumMode(DISABLE_SPECTRUM);
			}
			else
			{
				changeSpectrumMode(spectrum);
			}
		}
		changeIlluminationPreset(illumination);
		changeVisualisationPreset(visualisation);

		changeModeResponceSetting(responce);
		changeScreenSamplingSetting(sampling);
		changeWhiteBalanceSetting(whiteBalance);
		changeLightBrightnessSetting(brightness);
		changeLightSensitivitySetting(sensitivity);
		changeLightingArrangementSetting(arrangement);
	}
}

System::~System(void)
{
	for(unsigned int processor = 0; processor < NUM_SCREEN_MODES; processor++)
	{
		delete processors_[processor];
	}
	delete processors_;

	delete amBX_;
	delete spectrum_;
	delete screen_;
}


void System::reinitialise(void)
{
	//worked fine after sleep mode on win 7 sp1 - disabling for now
/*	if(synesthesiaInitialised_) {
		//Odd issue takes place after restart preventing FMOD and Synesthesia from working, but neither produce any errors.
		changeSpectrumMode(DISABLE_SPECTRUM);
		synesthesiaInitialised_ = false;
	}*/
}

bool System::successfullyInitialised(void)
{
	return systemInitialised_;
}

bool System::usingAurora(void)
{
	return auroraInitialised_;
}

bool System::usingSynesthesia(void)
{
	return synesthesiaInitialised_;
}

void System::changeLightingState(LightingState state)
{
	switch(state)
	{
		case LIGHTS_ENABLED:
			amBX_ = new AmbxEngine(updateTime_, systemInitialised_);
			amBXDisabled_ = !systemInitialised_;

			if(!systemInitialised_)
			{
				MessageBox(NULL, L"amBX could not be detected on your computer. Make sure the amBX service is operating before restarting this program. Press OK to close.\n", L"amBX Initialisation Failed", MB_OK | MB_ICONEXCLAMATION);
			}
			else
			{
				changeScreenMode(screenMode_);
				changeSpectrumMode(spectrumMode_);

				changeModeResponceSetting(modeResponceSetting_);
				changeWhiteBalanceSetting(whiteBalanceSetting_);
				changeLightBrightnessSetting(lightBrightnessSetting_);
				changeLightingArrangementSetting(lightingArrangementSetting_);
			}
			break;
		case LIGHTS_DISABLED:
			ScreenMode savedScreenMode = screenMode_;
			SpectrumMode savedSpectrumMode = spectrumMode_;

			changeScreenMode(DISABLE_SCREEN);
			changeSpectrumMode(DISABLE_SPECTRUM);

			screenMode_ = savedScreenMode;
			spectrumMode_ = savedSpectrumMode;

			delete amBX_;
			amBX_ = NULL;
			amBXDisabled_ = true;
			break;
	}

	lightingState_ = state;
}

void System::changeScreenMode(ScreenMode mode)
{
	bool changeSuccessful = false;

	if(!amBXDisabled_)
	{
		switch(mode)
		{
			case AVERAGE_SCREEN:
				changeSuccessful = screen_->changePixelProcessor(processors_[AVERAGE_SCREEN]);
				screen_->disableIlluminateEmulation();
				screen_->reinitialiseScreen();
				break;
			case VIBRANT_SCREEN:
				changeSuccessful = screen_->changePixelProcessor(processors_[VIBRANT_SCREEN]);
				screen_->disableIlluminateEmulation();
				screen_->reinitialiseScreen();
				break;
			case ILLUMINATE_SCREEN:
				changeSuccessful = screen_->changePixelProcessor(processors_[ILLUMINATE_SCREEN]);
				screen_->enableIlluminateEmulation();
				break;
			case DISABLE_SCREEN:
				changeSuccessful = screen_->changePixelProcessor(processors_[DISABLE_SCREEN]);
				screen_->disableIlluminateEmulation();
				break;
		}
	}
	else
	{
		changeSuccessful = true;
	}

	if(changeSuccessful)
	{
		screenMode_ = mode;
	}
	else
	{
		screenMode_ = DISABLE_SCREEN;
		screen_->disableIlluminateEmulation();
	}
}

void System::changeSpectrumMode(SpectrumMode mode)
{
	if(!amBXDisabled_)
	{
		switch(mode)
		{
			case AMBIENT_SPECTRUM:
				amBX_->setSaturation(HYBRID_SATURATION);
				break;
			case REACTIVE_SPECTRUM:
				amBX_->setSaturation(HYBRID_SATURATION);
				break;
			case IMMERSIVE_SPECTRUM:
				amBX_->setSaturation(SYNESTHESIA_SATURATION);
				break;
			case DISABLE_SPECTRUM:
				amBX_->setSaturation(DEFAULT_SATURATION);
				break;
		}
	}

	spectrumMode_ = mode;
}

void System::changeIlluminationPreset(IlluminationPreset preset)
{
	bool illuminateEmulated = (screenMode_ == ILLUMINATE_SCREEN);
	switch(preset)
	{
		case ORIGINAL_ILLUMINATION:
			if(illuminateEmulated){screen_->disableIlluminateEmulation();}

			screen_->excludeAllColourPeriods();

			screen_->includeColourPeriod(HSVColour(234.0f, 0.7f, 0.7f), HSVColour(162.0f, 0.7f, 0.7f), 6000, 3000, 6000);
			screen_->includeColourPeriod(HSVColour(162.0f, 0.7f, 0.7f), HSVColour(90.0f, 0.7f, 0.7f), 6000, 3000, 6000);
			screen_->includeColourPeriod(HSVColour(90.0f, 0.7f, 0.7f), HSVColour(18.0f, 0.7f, 0.7f), 6000, 3000, 6000);
			screen_->includeColourPeriod(HSVColour(18.0f, 0.7f, 0.7f), HSVColour(306.0f, 0.7f, 0.7f), 6000, 3000, 6000);
			screen_->includeColourPeriod(HSVColour(306.0f, 0.7f, 0.7f), HSVColour(234.0f, 0.7f, 0.7f), 6000, 3000, 6000);

			if(illuminateEmulated){screen_->enableIlluminateEmulation();}

			illuminationPreset_ = preset;
			break;
		case SPECTRUM_ILLUMINATION:
			if(illuminateEmulated){screen_->disableIlluminateEmulation();}

			screen_->excludeAllColourPeriods();

			screen_->includeColourPeriod(HSVColour(0.0f, 1.0f, 0.8f), HSVColour(60.0f, 1.0f, 0.8f), 0, 15000, 0);
			screen_->includeColourPeriod(HSVColour(60.0f, 1.0f, 0.8f), HSVColour(120.0f, 1.0f, 0.8f), 0, 15000, 0);
			screen_->includeColourPeriod(HSVColour(120.0f, 1.0f, 0.8f), HSVColour(180.0f, 1.0f, 0.8f), 0, 15000, 0);
			screen_->includeColourPeriod(HSVColour(180.0f, 1.0f, 0.8f), HSVColour(240.0f, 1.0f, 0.8f), 0, 15000, 0);
			screen_->includeColourPeriod(HSVColour(240.0f, 1.0f, 0.8f), HSVColour(300.0f, 1.0f, 0.8f), 0, 15000, 0);
			screen_->includeColourPeriod(HSVColour(300.0f, 1.0f, 0.8f), HSVColour(0.0f, 1.0f, 0.8f), 0, 15000, 0);

			if(illuminateEmulated){screen_->enableIlluminateEmulation();}

			illuminationPreset_ = preset;
			break;
		case CANDLE_ILLUMINATION:
			if(illuminateEmulated){screen_->disableIlluminateEmulation();}

			screen_->excludeAllColourPeriods();

			screen_->includeColourPeriod(HSVColour(38.5f, 0.928f, 0.986f), HSVColour(38.5f, 0.871f, 0.693f), 0, 1000, 0);
			screen_->includeColourPeriod(HSVColour(38.5f, 0.871f, 0.693f), HSVColour(30.6f, 0.921f, 0.564f), 0, 1000, 0);
			screen_->includeColourPeriod(HSVColour(30.6f, 0.921f, 0.564f), HSVColour(36.7f, 0.871f, 0.686f), 0, 1000, 0);
			screen_->includeColourPeriod(HSVColour(36.7f, 0.871f, 0.686f), HSVColour(41.0f, 0.951f, 0.964f), 0, 1000, 0);
			screen_->includeColourPeriod(HSVColour(41.0f, 0.951f, 0.964f), HSVColour(38.5f, 0.928f, 0.986f), 0, 1000, 0);

			if(illuminateEmulated){screen_->enableIlluminateEmulation();}

			illuminationPreset_ = preset;
			break;
		case RELAX_ILLUMINATION:
			if(illuminateEmulated){screen_->disableIlluminateEmulation();}

			screen_->excludeAllColourPeriods();

			screen_->includeColourPeriod(HSVColour(229.0f, 0.714f, 0.546f), HSVColour(172.4f, 0.628f, 0.513f), 2000, 8000, 2000);
			screen_->includeColourPeriod(HSVColour(172.4f, 0.628f, 0.513f), HSVColour(126.0f, 0.800f, 0.556f), 2000, 8000, 2000);
			screen_->includeColourPeriod(HSVColour(126.0f, 0.800f, 0.556f), HSVColour(198.0f, 0.536f, 0.482f), 2000, 8000, 2000);
			screen_->includeColourPeriod(HSVColour(198.0f, 0.536f, 0.482f), HSVColour(295.6f, 0.771f, 0.513f), 2000, 8000, 2000);
			screen_->includeColourPeriod(HSVColour(295.6f, 0.771f, 0.513f), HSVColour(229.0f, 0.714f, 0.546f), 2000, 8000, 2000);

			if(illuminateEmulated){screen_->enableIlluminateEmulation();}

			illuminationPreset_ = preset;
			break;	
		case CUSTOM_ILLUMINATION:
			break;
	}
}

void System::changeVisualisationPreset(VisualisationPreset preset)
{
	switch(preset)
	{
		case NATURAL_VISUALISATION:
			spectrum_->excludeAllColourBands();

			spectrum_->includeColourBand(RGBColour(1.0f, 0.0f, 0.0f), ResponceBand(22, 220), 0.8f);
			spectrum_->includeColourBand(RGBColour(0.0f, 1.0f, 0.0f), ResponceBand(220, 2200), 1.0f);
			spectrum_->includeColourBand(RGBColour(0.0f, 0.0f, 1.0f), ResponceBand(2200, 22000), 1.3f);

			visualisationPreset_ = preset;
			break;
		case LIQUID_VISUALISATION:
			spectrum_->excludeAllColourBands();

			spectrum_->includeColourBand(RGBColour(0.0f, 1.0f, 0.0f), ResponceBand(22, 220), 0.8f);
			spectrum_->includeColourBand(RGBColour(0.0f, 0.0f, 1.0f), ResponceBand(220, 2200), 1.0f);
			spectrum_->includeColourBand(RGBColour(1.0f, 0.0f, 0.0f), ResponceBand(2200, 22000), 1.3f);

			visualisationPreset_ = preset;
			break;
		case ENERGY_VISUALISATION:
			spectrum_->excludeAllColourBands();

			spectrum_->includeColourBand(RGBColour(0.0f, 0.0f, 1.0f), ResponceBand(22, 220), 0.8f);
			spectrum_->includeColourBand(RGBColour(1.0f, 0.0f, 0.0f), ResponceBand(220, 2200), 1.0f);
			spectrum_->includeColourBand(RGBColour(0.0f, 1.0f, 0.0f), ResponceBand(2200, 22000), 1.3f);

			visualisationPreset_ = preset;
			break;
		case CUSTOM_VISUALISATION:
			break;
	}
}

void System::changeModeResponceSetting(ModeResponceSetting setting)
{
	switch(setting)
	{
		case LOW_RESPONCE:
			spectrum_->modifySpectrumResponce(0.5f);
			if(!amBXDisabled_)
			{
				amBX_->changeFadeTime(LOW_UPDATE_TIME);
			}
			updateTime_ = LOW_UPDATE_TIME;
			break;
		case STANDARD_RESPONCE:
			spectrum_->modifySpectrumResponce(1.0f);
			if(!amBXDisabled_)
			{
				amBX_->changeFadeTime(STANDARD_UPDATE_TIME);
			}
			updateTime_ = STANDARD_UPDATE_TIME;
			break;
		case HIGH_RESPONCE:
			spectrum_->modifySpectrumResponce(2.0f);
			if(!amBXDisabled_)
			{
				amBX_->changeFadeTime(HIGH_UPDATE_TIME);
			}
			updateTime_ = HIGH_UPDATE_TIME;
			break;
	}

	modeResponceSetting_ = setting;	
}

void System::changeScreenSamplingSetting(ScreenSamplingSetting setting)
{
	switch(setting)
	{
		case LOW_SAMPLING:
			screen_->modifySampleSize(0.5f);
			break;
		case STANDARD_SAMPLING:
			screen_->modifySampleSize(1.0f);
			break;
		case HIGH_SAMPLING:
			screen_->modifySampleSize(2.0f);
			break;
	}

	screenSamplingSetting_ = setting;	
}

void System::changeWhiteBalanceSetting(WhiteBalanceSetting setting)
{
	if(!amBXDisabled_)
	{
		switch(setting)
		{
			case OFF_WHITE_BALANCE:
				amBX_->setWhiteBalance(OFF_WHITE_BALANCE * WHITE_BALANCE_SCALING);
				break;
			case ONE_WHITE_BALANCE:
				amBX_->setWhiteBalance(ONE_WHITE_BALANCE * WHITE_BALANCE_SCALING);
				break;
			case TWO_WHITE_BALANCE:
				amBX_->setWhiteBalance(TWO_WHITE_BALANCE * WHITE_BALANCE_SCALING);
				break;
			case THREE_WHITE_BALANCE:
				amBX_->setWhiteBalance(THREE_WHITE_BALANCE * WHITE_BALANCE_SCALING);
				break;
			case FOUR_WHITE_BALANCE:
				amBX_->setWhiteBalance(FOUR_WHITE_BALANCE * WHITE_BALANCE_SCALING);
				break;
			case FIVE_WHITE_BALANCE:
				amBX_->setWhiteBalance(FIVE_WHITE_BALANCE * WHITE_BALANCE_SCALING);
				break;
		}
	}

	whiteBalanceSetting_ = setting;
}

void System::changeLightBrightnessSetting(LightBrightnessSetting setting)
{
	if(!amBXDisabled_)
	{
		switch(setting)
		{
			case TEN_BRIGHTNESS:
				amBX_->setBrightness(0.1f);
				break;
			case TWENTY_BRIGHTNESS:
				amBX_->setBrightness(0.2f);
				break;
			case THIRTY_BRIGHTNESS:
				amBX_->setBrightness(0.3f);
				break;
			case FOURTY_BRIGHTNESS:
				amBX_->setBrightness(0.4f);
				break;
			case FIFTY_BRIGHTNESS:
				amBX_->setBrightness(0.5f);
				break;
			case SIXTY_BRIGHTNESS:
				amBX_->setBrightness(0.6f);
				break;
			case SEVENTY_BRIGHTNESS:
				amBX_->setBrightness(0.7f);
				break;
			case EIGHTY_BRIGHTNESS:
				amBX_->setBrightness(0.8f);
				break;
			case NINETY_BRIGHTNESS:
				amBX_->setBrightness(0.9f);
				break;
			case ONEHUNDRED_BRIGHTNESS:
				amBX_->setBrightness(1.0f);
				break;
		}
	}

	lightBrightnessSetting_ = setting;
}

void System::changeLightSensitivitySetting(LightSensitivitySetting setting)
{
	switch(setting)
	{
		case PFIVE_SENSITIVITY:
			spectrum_->disableAdaptiveVolume();
			spectrum_->setVolume(pow(SENSITIVITY_MULTIPLIER, PFIVE_SENSITIVITY));
			break;
		case ONE_SENSITIVITY:
			spectrum_->disableAdaptiveVolume();
			spectrum_->setVolume(pow(SENSITIVITY_MULTIPLIER, ONE_SENSITIVITY));
			break;
		case TWO_SENSITIVITY:
			spectrum_->disableAdaptiveVolume();
			spectrum_->setVolume(pow(SENSITIVITY_MULTIPLIER, TWO_SENSITIVITY));
			break;
		case FOUR_SENSITIVITY:
			spectrum_->disableAdaptiveVolume();
			spectrum_->setVolume(pow(SENSITIVITY_MULTIPLIER, FOUR_SENSITIVITY));
			break;
		case EIGHT_SENSITIVITY:
			spectrum_->disableAdaptiveVolume();
			spectrum_->setVolume(pow(SENSITIVITY_MULTIPLIER, EIGHT_SENSITIVITY));
			break;
		case SIXTEEN_SENSITIVITY:
			spectrum_->disableAdaptiveVolume();
			spectrum_->setVolume(pow(SENSITIVITY_MULTIPLIER, SIXTEEN_SENSITIVITY));
			break;
		case THIRTYTWO_SENSITIVITY:
			spectrum_->disableAdaptiveVolume();
			spectrum_->setVolume(pow(SENSITIVITY_MULTIPLIER, THIRTYTWO_SENSITIVITY));
			break;
		case SIXTYFOUR_SENSITIVITY:
			spectrum_->disableAdaptiveVolume();
			spectrum_->setVolume(pow(SENSITIVITY_MULTIPLIER, SIXTYFOUR_SENSITIVITY));
			break;
		case ONETWOEIGHT_SENSITIVITY:
			spectrum_->disableAdaptiveVolume();
			spectrum_->setVolume(pow(SENSITIVITY_MULTIPLIER, ONETWOEIGHT_SENSITIVITY));
			break;
		case TWOFIVESIX_SENSITIVITY:
			spectrum_->disableAdaptiveVolume();
			spectrum_->setVolume(pow(SENSITIVITY_MULTIPLIER, TWOFIVESIX_SENSITIVITY));
			break;
		case ADAPTIVE_SENSITIVITY:
			spectrum_->enableAdaptiveVolume();
			break;
	}

	lightSensitivitySetting_ = setting;
}

void System::changeLightingArrangementSetting(LightingArrangementSetting setting)
{
	switch(setting)
	{
		case STEREO_ARRANGEMENT:
			if(!amBXDisabled_)
			{
				amBX_->disableSurroundEffect();
			}
			break;
		case SURROUND_ARRANGEMENT:
			if(!amBXDisabled_)
			{
				amBX_->enableSurroundEffect();
			}
			break;
	}

	lightingArrangementSetting_ = setting;			
}

LightingState System::currentLightingState(void)
{
	return lightingState_;
}

ScreenMode System::currentScreenMode(void)
{
	return screenMode_;
}

SpectrumMode System::currentSpectrumMode(void)
{
	return spectrumMode_;
}

IlluminationPreset System::currentIlluminationPreset(void)
{
	return illuminationPreset_;
}

VisualisationPreset System::currentVisualisationPreset(void)
{
	return visualisationPreset_;
}

ModeResponceSetting System::currentModeResponceSetting(void)
{
	return modeResponceSetting_;
}

ScreenSamplingSetting System::currentScreenSamplingSetting(void)
{
	return screenSamplingSetting_;
}

WhiteBalanceSetting System::currentWhiteBalanceSetting(void)
{
	return whiteBalanceSetting_;
}

LightBrightnessSetting System::currentLightBrightnessSetting(void)
{
	return lightBrightnessSetting_;
}

LightSensitivitySetting System::currentLightSensitivitySetting(void)
{
	return lightSensitivitySetting_;
}

LightingArrangementSetting System::currentLightingArrangementSetting(void)
{
	return lightingArrangementSetting_;
}


void System::execute(void)
{
	//
	RGBColour left;
	RGBColour right;
	RGBColour centre;
	RGBColour iconColour;

	//
	list<RGBColour> pulses;
	list<PositionAngle> angles;

	//
	DWORD startTime;
	DWORD endTime;

	//
	MSG msg;

	//
	while(systemInitialised_)
	{
		// Check to see if any messages are waiting in the queue
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//Translate the message and dispatch it to WindowProc()
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If the message is WM_QUIT, exit the while loop
		if(msg.message == WM_QUIT)
		{
			systemInitialised_ = false;
		}
		else
		{
			startTime = GetTickCount();

			if(!amBXDisabled_)
			{
				if(spectrumMode_ != DISABLE_SPECTRUM)
				{
					spectrum_->processAudio();
				}

				if(screen_->desktopCompositionPresent())
				{
					changeScreenMode(screenMode_);
				}

				if((screenMode_ == ILLUMINATE_SCREEN) || (spectrumMode_ != IMMERSIVE_SPECTRUM))
				{
					unsigned int screenAttempts = 0;

					screen_->advanceIlluminateEmulation(updateTime_);

					do {
						//Reinitialise the screen capturer in the event that the screen
						//has changed and it is no longer able to get pixel data from it.
						if(screen_->invalidScreen()) {
							screen_->reinitialiseScreen();
						}

						screen_->acquireScreenRegion(centre, ScreenRegion(0.333f, 0.667f, 0.0f, 1.0f));
						screen_->acquireScreenRegion(left, ScreenRegion(0.0f, 0.333f, 0.0f, 1.0f));
						screen_->acquireScreenRegion(right, ScreenRegion(0.667f, 1.0f, 0.0f, 1.0f));

						screenAttempts++;
					} while (screen_->invalidScreen() && (screenAttempts < MAX_SCREEN_ATTEMPTS));

					amBX_->setAmbient(left, right, centre);
				}
				else
				{
					amBX_->setAmbient(RGBColour(), RGBColour(), RGBColour());
				}

				if((screenMode_ != ILLUMINATE_SCREEN)  || (spectrumMode_ == IMMERSIVE_SPECTRUM))
				{
					if((spectrumMode_ == AMBIENT_SPECTRUM) || (spectrumMode_ == IMMERSIVE_SPECTRUM))
					{
						spectrum_->acquireColourResponces(left, right);

						amBX_->addToAmbient(left, right);
					}
					
					if((spectrumMode_ == REACTIVE_SPECTRUM) || (spectrumMode_ == IMMERSIVE_SPECTRUM))
					{
						spectrum_->acquireColourPulses(pulses, angles);

						amBX_->addColourPulses(pulses, angles);
					}
				}


				amBX_->updateLighting(updateTime_);

				amBX_->getLightColour(iconColour, NORTH);
			}
			else
			{
				iconColour = RGBColour();
			}

			icon_->representColour(iconColour, updateTime_);

			endTime = GetTickCount();

			DWORD elapsed = endTime - startTime;

			if(elapsed < (DWORD)updateTime_)
			{
				Sleep((DWORD)updateTime_ - elapsed);
			}
		}
	}
}

bool System::saveSettings(void) {
	if (!iniPath[0]) {
		return false;
	}
	CSimpleIniA ini(false, true, true);

//Modes
	const char *value;
	switch (screenMode_) {
	case AVERAGE_SCREEN:
		value = "Average";
		break;
	case ILLUMINATE_SCREEN:
		value = "Illuminate";
		break;
	case DISABLE_SCREEN:
		value = "Disable";
		break;
	default:
	case VIBRANT_SCREEN:
		value = "Vibrant";
		break;
	}
	ini.SetValue("Modes", "Aurora", value);
	switch (spectrumMode_) {
	case AMBIENT_SPECTRUM:
		value = "Ambient";
		break;
	case IMMERSIVE_SPECTRUM:
		value = "Immersive";
		break;
	case DISABLE_SPECTRUM:
		value = "Disable";
		break;
	default:
	case REACTIVE_SPECTRUM:
		value = "Reactive";
		break;
	}
	ini.SetValue("Modes", "Synesthesia", value);
//Presets
	switch (illuminationPreset_) {
	case SPECTRUM_ILLUMINATION:
		value = "Spectrum";
		break;
	case CANDLE_ILLUMINATION:
		value = "Candle";
		break;
	case RELAX_ILLUMINATION:
		value = "Relax";
		break;
	default:
	case ORIGINAL_ILLUMINATION:
		value = "Original";
		break;
	}
	ini.SetValue("Presets", "Illumination", value);
	switch (visualisationPreset_) {
	case LIQUID_VISUALISATION:
		value = "Liquid";
		break;
	case ENERGY_VISUALISATION:
		value = "Energy";
		break;
	default:
	case NATURAL_VISUALISATION:
		value = "Natural";
		break;
	}
	ini.SetValue("Presets", "Visualisation", value);
//Settings
	switch (modeResponceSetting_) {
	case LOW_RESPONCE:
		value = "Low";
		break;
	case HIGH_RESPONCE:
		value = "High";
		break;
	default:
	case STANDARD_RESPONCE:
		value = "Standard";
		break;
	}
	ini.SetValue("Settings", "ModeResponse", value);
	switch (screenSamplingSetting_) {
	case LOW_SAMPLING:
		value = "Low";
		break;
	case HIGH_SAMPLING:
		value = "High";
		break;
	default:
	case STANDARD_SAMPLING:
		value = "Standard";
		break;
	}
	ini.SetValue("Settings", "ScreenSampling", value);
	switch (lightSensitivitySetting_) {
	case PFIVE_SENSITIVITY:
		value = "0.5";
		break;
	case ONE_SENSITIVITY:
		value = "1";
		break;
	case TWO_SENSITIVITY:
		value = "2";
		break;
	case FOUR_SENSITIVITY:
		value = "4";
		break;
	case EIGHT_SENSITIVITY:
		value = "8";
		break;
	case SIXTEEN_SENSITIVITY:
		value = "16";
		break;
	case THIRTYTWO_SENSITIVITY:
		value = "32";
		break;
	case SIXTYFOUR_SENSITIVITY:
		value = "64";
		break;
	case ONETWOEIGHT_SENSITIVITY:
		value = "128";
		break;
	case TWOFIVESIX_SENSITIVITY:
		value = "256";
		break;
	default:
	case ADAPTIVE_SENSITIVITY:
		value = "Delta";
		break;
	}
	ini.SetValue("Settings", "SoundSensitivity", value);
	switch (whiteBalanceSetting_) {
	case OFF_WHITE_BALANCE:
		value = "0";
		break;
	case ONE_WHITE_BALANCE:
		value = "1";
		break;
	case THREE_WHITE_BALANCE:
		value = "3";
		break;
	case FOUR_WHITE_BALANCE:
		value = "4";
		break;
	case FIVE_WHITE_BALANCE:
		value = "5";
		break;
	default:
	case TWO_WHITE_BALANCE:
		value = "2";
		break;
	}
	ini.SetValue("Settings", "WhiteBalance", value);
	switch (lightBrightnessSetting_) {
	case TEN_BRIGHTNESS:
		value = "10";
		break;
	case TWENTY_BRIGHTNESS:
		value = "20";
		break;
	case THIRTY_BRIGHTNESS:
		value = "30";
		break;
	case FOURTY_BRIGHTNESS:
		value = "40";
		break;
	case FIFTY_BRIGHTNESS:
		value = "50";
		break;
	case SIXTY_BRIGHTNESS:
		value = "60";
		break;
	case SEVENTY_BRIGHTNESS:
		value = "70";
		break;
	case EIGHTY_BRIGHTNESS:
		value = "80";
		break;
	case NINETY_BRIGHTNESS:
		value = "90";
		break;
	default:
	case ONEHUNDRED_BRIGHTNESS:
		value = "100";
		break;
	}
	ini.SetValue("Settings", "LightBrightness", value);
	switch (lightingArrangementSetting_){
	case SURROUND_ARRANGEMENT:
		value = "Surround";
		break;
	default:
	case STEREO_ARRANGEMENT:
		value = "Stereo";
		break;
	}
	ini.SetValue("Settings", "LightingArrangement", value);

	SI_Error rc = ini.SaveFile(iniPath, false);
	if (rc < 0) {
		return false;
	}
	return true;
}