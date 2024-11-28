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
#if 1
	// Official synthv1's process procedure. Supports multi-channel.
	// Adapted from synthv1_jack.cpp (synthv1_jack::process()).

	const uint16_t nchannels = synthv1::channels();
	uint32_t ndelta = 0;

	for (uint32_t event_index = 0; event_index < midiEventCount; ++event_index)
	{
		const MidiEvent &event = midiEvents[event_index];

		if (event.frame > ndelta)
		{
			const uint32_t nread = event.frame - ndelta;
			if (nread > 0)
			{
				synthv1::process(const_cast<float**>(inputs), outputs, nread);
				for (uint16_t k = 0; k < nchannels; ++k) {
					inputs[k]  += nread;
					outputs[k] += nread;
				}
			}
		}

		// Handle SysEx event (e.g. Panic).
		_checkAndHandleSysExEvent(event);

		// Handle all MIDI events.
		synthv1::process_midi(const_cast<uint8_t*>(event.data), event.size);
	}

	if (nframes > ndelta)
		synthv1::process(const_cast<float**>(inputs), outputs, nframes - ndelta);

#else
	// Classic way to render the synthesizer.
	// Stable and fluent enough, but I still prefer rncbc's standard procedure.
	for (uint32_t event_index = 0; event_index < midiEventCount; ++event_index)
	{
		// Handle panic (SysEx "F0 7B") event.
		_checkAndHandlePanicEvent(midiEvents[event_index]);

		// Handle all MIDI events.
		synthv1::process_midi(const_cast<uint8_t*>(midiEvents[event_index].data), midiEvents[event_index].size);
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
void synthv1_dpf::_checkAndHandleSysExEvent(const MidiEvent& currentEvent)
{
	const int status  = (currentEvent.data[0] & 0xf0);
	const int key = (currentEvent.data[1] & 0x7f);

	if (status == 0xf0 && key == 0x7b)	[[unlikely]] // SysEx Panic command
	{
		d_stderr2("Panic command (F0 7B) detected");
		synthv1::reset();
	}
	else if (status == 0xf0 && key == 0x7e)  [[unlikely]]
	{
		// d_stderr2("Action request 'before loading preset' (F0 7E) detected");

		fRunningState = this->running(false);

		this->setTuningEnabled(false);
		this->reset();
	}
	else if (status == 0xf0 && key == 0x7f)  [[unlikely]]
	{
		// d_stderr2("Action request 'after loading preset' (F0 7F) detected");

		this->stabilize();
		this->reset();
		this->running(fRunningState);
	}
	else if (status == 0xf0 && key == 0x7c)  [[unlikely]]
	{
		d_stderr2("Stabilize command (F0 7C) detected");

		this->stabilize();
	}
}

//-------------------------------------------------------------------------
// SynthV1Plugin - DPF plugin interface.
//

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

	parameter.name = synthv1_param::paramFullName(currentParam);
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

	// Measure BPM, then apply to synthesizer
	const TimePosition& timePos(getTimePosition());
	if (timePos.bbt.valid)
	{
		const float host_bpm = timePos.bbt.beatsPerMinute;
		if (::fabsf(host_bpm - fSynthesizer->tempo()) > 0.001f)
			fSynthesizer->setTempo(host_bpm);
	}
}

void SynthV1Plugin::sampleRateChanged(double newSampleRate)
{
	DISTRHO_SAFE_ASSERT_RETURN(fSynthesizer != nullptr, )

	fSynthesizer->setSampleRate(newSampleRate);
}


/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

START_NAMESPACE_DISTRHO

Plugin *createPlugin()
{
	d_stderr2(">> Creating plugin...");
	return new SynthV1Plugin();
}

END_NAMESPACE_DISTRHO

// end of synthv1_dpf.cpp
