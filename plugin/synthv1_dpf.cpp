// synthv1_dpf.cpp
//
/****************************************************************************
   Copyright (C) 2023-2024, AnClark Liu. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#include "synthv1_dpf.h"
#include "synthv1_param.h"

//-------------------------------------------------------------------------
// synthv1_dpf - Constants.
//

// Parameter names - extracted from LV2 definition
static const char *ParamNames[synthv1::NUM_PARAMS] = {
	"DCO1 Wave Shape 1",
	"DCO1 Wave Width 1",
	"DCO1 Wave Bandlimit 1",
	"DCO1 Wave Sync 1",
	"DCO1 Wave Shape 2",
	"DCO1 Width 2",
	"DCO1 Wave Bandlimit 2",
	"DCO1 Wave Sync 2",
	"DCO1 Balance",
	"DCO1 Detune",
	"DCO1 Phase",
	"DCO1 Ring Mod",
	"DCO1 Octave",
	"DCO1 Tuning",
	"DCO1 Glide",
	"DCO1 Env.Time",
	"DCF1 Enabled",
	"DCF1 Cutoff",
	"DCF1 Resonance",
	"DCF1 Type",
	"DCF1 Slope",
	"DCF1 Envelope",
	"DCF1 Attack",
	"DCF1 Decay",
	"DCF1 Sustain",
	"DCF1 Release",
	"LFO1 Enabled",
	"LFO1 Wave Shape",
	"LFO1 Wave Width",
	"LFO1 BPM",
	"LFO1 Rate",
	"LFO1 Sync",
	"LFO1 Sweep",
	"LFO1 Pitch",
	"LFO1 Balance",
	"LFO1 Ring Mod",
	"LFO1 Cutoff",
	"LFO1 Resonance",
	"LFO1 Panning",
	"LFO1 Volume",
	"LFO1 Attack",
	"LFO1 Decay",
	"LFO1 Sustain",
	"LFO1 Release",
	"DCA1 Volume",
	"DCA1 Attack",
	"DCA1 Decay",
	"DCA1 Sustain",
	"DCA1 Release",
	"OUT1 Stereo Width",
	"OUT1 Panning",
	"OUT1 FX Send",
	"OUT1 Volume",
	"DEF1 Pitchbend",
	"DEF1 Modwheel",
	"DEF1 Pressure",
	"DEF1 Velocity",
	"DEF1 Channel",
	"DEF1 Mono",
	"DCO2 Wave Shape 1",
	"DCO2 Wave Width 1",
	"DCO2 Wave Bandlimit 1",
	"DCO2 Wave Sync 1",
	"DCO2 Wave Shape 2",
	"DCO2 Wave Width 2",
	"DCO2 Wave Bandlimit 2",
	"DCO2 Wave Sync 2",
	"DCO2 Balance",
	"DCO2 Detune",
	"DCO2 Phase",
	"DCO2 Ring Mod",
	"DCO2 Octave",
	"DCO2 Tuning",
	"DCO2 Glide",
	"DCO2 Env.Time",
	"DCF2 Enabled",
	"DCF2 Cutoff",
	"DCF2 Resonance",
	"DCF2 Type",
	"DCF2 Slope",
	"DCF2 Envelope",
	"DCF2 Attack",
	"DCF2 Decay",
	"DCF2 Sustain",
	"DCF2 Release",
	"LFO2 Enabled",
	"LFO2 Wave Shape",
	"LFO2 Wave Width",
	"LFO2 BPM",
	"LFO2 Rate",
	"LFO2 Sync",
	"LFO2 Sweep",
	"LFO2 Pitch",
	"LFO2 Balance",
	"LFO2 Ring Mod",
	"LFO2 Cutoff",
	"LFO2 Resonance",
	"LFO2 Panning",
	"LFO2 Volume",
	"LFO2 Attack",
	"LFO2 Decay",
	"LFO2 Sustain",
	"LFO2 Release",
	"DCA2 Volume",
	"DCA2 Attack",
	"DCA2 Decay",
	"DCA2 Sustain",
	"DCA2 Release",
	"OUT2 Stereo Width",
	"OUT2 Panning",
	"OUT2 FX Send",
	"OUT2 Volume",
	"DEF2 Pitchbend",
	"DEF2 Modwheel",
	"DEF2 Pressure",
	"DEF2 Velocity",
	"DEF2 Channel",
	"DEF2 Mono",
	"Chorus Wet",
	"Chorus Delay",
	"Chorus Feedback",
	"Chorus Rate",
	"Chorus Modulation",
	"Flanger Wet",
	"Flanger Delay",
	"Flanger Feedback",
	"Flanger Daft",
	"Phaser Wet",
	"Phaser Rate",
	"Phaser Feedback",
	"Phaser Depth",
	"Phaser Daft",
	"Delay Wet",
	"Delay Delay",
	"Delay Feedback",
	"Delay BPM",
	"Reverb Wet",
	"Reverb Room",
	"Reverb Damp",
	"Reverb Feedback",
	"Reverb Width",
	"Dynamic Compressor",
	"Dynamic Limiter",
	"Keyboard Low",
	"Keyboard High"
};

//-------------------------------------------------------------------------
// synthv1_dpf - Instantiation and cleanup.
//

synthv1_dpf::synthv1_dpf(double sample_rate) : synthv1(2, float(sample_rate))
{
}

synthv1_dpf::~synthv1_dpf()
{
}

//-------------------------------------------------------------------------
// synthv1_dpf - methods impl.
//

void synthv1_dpf::updatePreset(bool /*bDirty*/)
{
	// NOTICE: No need to tell DPF about preset changes, since DPF knows it
	//         when parameter changes from UI side.
	//         Also "synthesizer -> plug-in" access is not essential in DPF.
	//         See: synthv1widget_dpf::updateParam (https://github.com/AnClark/synthv1-universal/blob/main/plugin/synthv1widget_dpf.cpp).
}

void synthv1_dpf::updateParam(synthv1::ParamIndex index)
{
	// NOTICE: No need to tell DPF about param changes. Reason mentioned above.
	(void)index;
}

void synthv1_dpf::updateParams(void)
{
	// NOTICE: No need to tell DPF about param changes. Reason mentioned above.
}

void synthv1_dpf::updateTuning(void)
{
	// NOTICE: DPF does not support manual tuning. Will not implement it for now.
}

void synthv1_dpf::activate(void)
{
	synthv1::reset();
}

void synthv1_dpf::deactivate(void)
{
	synthv1::reset();
}

void synthv1_dpf::run(const float **inputs, float **outputs, uint32_t nframes, const MidiEvent *midiEvents,
					  uint32_t midiEventCount)
{
#if 0	// WARNING: AMSH is fast, but it makes glide effect unavailable! So I disabled it.
	for (AudioMidiSyncHelper amsh(outputs, nframes, midiEvents, midiEventCount); amsh.nextEvent();)
	{
		for (uint32_t i = 0; i < amsh.midiEventCount; ++i)
		{
			const MidiEvent &ev(amsh.midiEvents[i]);
			synthv1::process_midi((uint8_t *)ev.data, ev.size);
		}

		synthv1::process((float **)inputs, amsh.outputs, amsh.frames);
	}
#else	// Now, use our classic, stable way to render the synthesizer.
    for (uint32_t event_index = 0; event_index < midiEventCount; ++event_index)
    {
		// Handle panic (SysEx "F0 7B") event.
		_checkAndHandlePanicEvent(midiEvents[event_index]);

		// Handle all MIDI events.
        synthv1::process_midi((uint8_t*)midiEvents[event_index].data, midiEvents[event_index].size);
    }

	synthv1::process(const_cast<float**>(inputs), outputs, nframes);
#endif
}

// NOTE:  Assigning "MIDI All Notes Off / All Sound Off" as panic message is just a workaround,
//        not a permanent solution.
//        That's because synthv1 allows re-assigning "MIDI All Notes Off / All Sound Off" CC.
//
//        In order not to break DPF's policy, a better way is to pass messages via SysEx.
//        (However, in future, to implement MIDI Learn, UI direct access may be essential!)
//
//        I define SysEx "F0 7B" as "panic" command. "7B" is the same as MIDI_ALL_NOTES_OFF.
void synthv1_dpf::_checkAndHandlePanicEvent(const MidiEvent& currentEvent)
{
	const int status  = (currentEvent.data[0] & 0xf0);
	const int key = (currentEvent.data[1] & 0x7f);

	if (status == 0xf0 && key == 0x7b)	[[unlikely]] // SysEx Panic command
	{
		d_stderr2("Panic command (F0 7B) detected");
		synthv1::reset();
	}
}

//-------------------------------------------------------------------------
// SynthV1Plugin - DPF plugin interface.
//

START_NAMESPACE_DISTRHO

SynthV1Plugin::SynthV1Plugin() : Plugin(synthv1::NUM_PARAMS, 0, 0) // parameters, programs, states
{
}

SynthV1Plugin::~SynthV1Plugin()
{
}

synthv1_dpf *SynthV1Plugin::getSynthesizer()
{
	DISTRHO_SAFE_ASSERT_RETURN(fSynthesizer != nullptr, nullptr)

	return &(*fSynthesizer); // Unique pointer ==> standard pointer
}

void SynthV1Plugin::initParameter(uint32_t index, Parameter &parameter)
{
	DISTRHO_SAFE_ASSERT_RETURN(fSynthesizer != nullptr, )

	synthv1::ParamIndex currentParam = (synthv1::ParamIndex)index;

	parameter.hints = kParameterIsAutomatable;

	parameter.name = ParamNames[index];
	parameter.shortName = synthv1_param::paramName(currentParam);
	parameter.symbol = synthv1_param::paramName(currentParam);
	parameter.ranges.def = synthv1_param::paramDefaultValue(currentParam);
	parameter.ranges.min = synthv1_param::paramMinValue(currentParam);
	parameter.ranges.max = synthv1_param::paramMaxValue(currentParam);

	if (synthv1_param::paramBool(currentParam))
	{
		parameter.hints |= kParameterIsBoolean;
	}
	else if (synthv1_param::paramInt(currentParam))
	{
		parameter.hints |= kParameterIsInteger;
	}

	// Apply default parameter values for synthv1 explicitly,
	// since DPF cannot apply it automatically
	fSynthesizer->setParamValue(currentParam, parameter.ranges.def);
}

float SynthV1Plugin::getParameterValue(uint32_t index) const
{
	DISTRHO_SAFE_ASSERT_RETURN(fSynthesizer != nullptr, 0.0f)

	return fSynthesizer->paramValue((synthv1::ParamIndex)index);
}

void SynthV1Plugin::setParameterValue(uint32_t index, float value)
{
	DISTRHO_SAFE_ASSERT_RETURN(fSynthesizer != nullptr, )

	fSynthesizer->setParamValue((synthv1::ParamIndex)index, value);
}

void SynthV1Plugin::activate()
{
	DISTRHO_SAFE_ASSERT_RETURN(fSynthesizer != nullptr, )

	fSynthesizer->activate();
}

void SynthV1Plugin::run(const float **inputs, float **outputs, uint32_t frames, const MidiEvent *midiEvents,
						uint32_t midiEventCount)
{
	DISTRHO_SAFE_ASSERT_RETURN(fSynthesizer != nullptr, )

	fSynthesizer->run(inputs, outputs, frames, midiEvents, midiEventCount);
}

void SynthV1Plugin::sampleRateChanged(double newSampleRate)
{
	DISTRHO_SAFE_ASSERT_RETURN(fSynthesizer != nullptr, )

	fSynthesizer->setSampleRate(newSampleRate);
}

Plugin *createPlugin()
{
	d_stderr2(">> Creating plugin...");
	return new SynthV1Plugin();
}

END_NAMESPACE_DISTRHO

// end of synthv1_dpf.cpp
