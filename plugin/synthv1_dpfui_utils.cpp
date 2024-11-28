#include "synthv1_config.h"
#include "synthv1_dpfui.h"
#include "synthv1_param.h"

#include <random>

// For get_system_config_dir()
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif


void SynthV1PluginUI::_randomParams()
{
	float p = 1.0f;

	synthv1_config *pConfig = synthv1_config::getInstance();
	if (pConfig)
		p = 0.01f * pConfig->fRandomizePercent;

#if 0
	if (QMessageBox::warning(this,
		tr("Warning"),
		tr("About to randomize current parameter values:\n\n"
		"-/+ %1%.\n\n"
		"Are you sure?").arg(100.0f * p),
		QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
		return;
#endif

	std::default_random_engine re(::time(nullptr));

	for (uint32_t i = 0; i < synthv1::NUM_PARAMS; ++i) {
		const synthv1::ParamIndex index = synthv1::ParamIndex(i);
		// Filter out some non-randomizable parameters!...
		if (index == synthv1::DCO1_OCTAVE  ||
			index == synthv1::DCO1_TUNING  ||
			index == synthv1::DCO1_ENVTIME ||
			index == synthv1::DCF1_ENABLED ||
			index == synthv1::LFO1_ENABLED) 
			continue;
		if (index >= synthv1::OUT1_WIDTH && index < synthv1::DCO2_SHAPE1)
			continue;
		if (index == synthv1::DCO2_OCTAVE  ||
			index == synthv1::DCO2_TUNING  ||
			index == synthv1::DCO2_ENVTIME ||
			index == synthv1::DCF2_ENABLED ||
			index == synthv1::LFO2_ENABLED) 
			continue;
		if (index >= synthv1::OUT2_WIDTH)
			break;

		{
			std::normal_distribution<float> nd;
			const float q = p * (synthv1_param::paramMaxValue(index) - synthv1_param::paramMinValue(index));
			float fValue = fParamStorage[index];

			if (synthv1_param::paramFloat(index))
				fValue += 0.5f * q * nd(re);
			else
				fValue = std::round(fValue + q * nd(re));
			if (fValue < synthv1_param::paramMinValue(index))
				fValue = synthv1_param::paramMinValue(index);
			else if (fValue > synthv1_param::paramMaxValue(index))
				fValue = synthv1_param::paramMaxValue(index);

			setParameterValue(index, fValue);
			parameterChanged(index, fValue);
		}
	}
}

void SynthV1PluginUI::_panic()
{
	// In official synthv1, UI directly invokes DSP's method to trigger a panic (synthv1::reset()).
	//
	// According to DPF's policy, it's not recommended to let the UI side directly access DSP side.
	// So a better way is to send a MIDI All Notes Off event, and invoke panic when detected the event.
	//
	// I will check and handle the event on synthv1_dpf::run().

	/**
	  SPECIAL NOTE ABOUT DISTRHO::UI::sendNote()

	  There is no API to send MIDI data to DSP side. Instead, DPF provides UI::sendNote().
	  But we can still send any MIDI data via this method, simply doing a hack.

	  In method UI::sendNote(uint8_t channel, uint8_t note, uint8_t velocity):

	  `channel` will be converted into MIDI data 0 by:
		`midi_data[0] = (velocity != 0 ? 0x90 : 0x80) | channel;`

	  To send MIDI controller data, we need to transform midi_data[0] into MIDI_STATUS_CONTROLLER (0xB0)
	  by simply applying a proper `channel` value. It would be easy to calculate:
			0x80 | channel = 0xB0
		=> 0b10000000 | channel = 0b10110000
		=> channel = 0b00110000
		=> channel = 0x30
	
	  Similarily, to send SysEx data, just do this transformation:
	       0x80 | channel = 0xF0
		=> 0b10000000 | channel = 0b11110000
		=> channel = 0b11110000
		=> channel = 0xf0
	*/
	sendNote(0xF0, 0x7B, 0);  //I defined SysEx "F0 7B" as Panic command. "7B" is the same as MIDI_ALL_NOTES_OFF.
}

void SynthV1PluginUI::_actionBeforeLoadingPreset()
{
	// I defined SysEx "F0 7E" as the command for actions before loading a preset.
	sendNote(0xF0, 0x7E, 0);
}

void SynthV1PluginUI::_actionAfterLoadingPreset()
{
	// I defined SysEx "F0 7F" as the command for actions after loading a preset.
	sendNote(0xF0, 0x7F, 0);
}

void SynthV1PluginUI::_requestStabilize()
{
	// I defined SysEx "F0 7C" as the command for requesting the synthesizer to stabilize.
	sendNote(0xF0, 0x7C, 0);
}

// --------------------------------------------------------------------------
// Global functions.

DISTRHO::String get_system_config_dir() {
#ifdef _WIN32
    // Windows
    char appdata[1024];
    DWORD size = sizeof(appdata);
    GetEnvironmentVariable("APPDATA", appdata, size);
    return DISTRHO::String(appdata);
#else
    // POSIX systems
    const char* config_home = getenv("XDG_CONFIG_HOME");
    if (config_home == nullptr) {
        // Fallback to ~/.config if XDG_CONFIG_HOME is not set
        char* home = getenv("HOME");
        if (home) {
            return DISTRHO::String(home) + "/.config";
        }
    }
    return DISTRHO::String(config_home);
#endif
}
