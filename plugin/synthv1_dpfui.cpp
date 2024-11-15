// synthv1_dpfui.cpp
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

#include "synthv1_dpfui.h"

#include "DearImGui/imgui.h"
#include "DistrhoPluginInfo.h"
#include "DistrhoUtils.hpp"

#include "synthv1.h"
#include "synthv1_dpf.h"

#include "veeone_wave_widget.hpp"
#include "veeone_filter_widget.hpp"
#include "veeone_adsr_widget.hpp"

//-------------------------------------------------------------------------
// SynthV1PluginUI - DPF Plugin UI interface.
//

START_NAMESPACE_DISTRHO

SynthV1PluginUI::SynthV1PluginUI()
    : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true),
      fResizeHandle(this)
{
    setGeometryConstraints(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true);

    // hide handle if UI is resizable
    if (isResizable())
        fResizeHandle.hide();

}

// ----------------------------------------------------------------------------------------------------------------
// DSP/Plugin Callbacks

void SynthV1PluginUI::parameterChanged(uint32_t index, float value)
{
    DISTRHO_SAFE_ASSERT_RETURN(index >= 0 && index < synthv1::NUM_PARAMS, )

    fParamStorage[index] = value;

    repaint();
}

// ----------------------------------------------------------------------------------------------------------------
// Widget Callbacks

void SynthV1PluginUI::onImGuiDisplay()
{
    const float width = getWidth();
    const float height = getHeight();
    const float margin = 20.0f * getScaleFactor();

    static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
    // Based on your use case you may want one or the other.
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    if (ImGui::Begin(DISTRHO_PLUGIN_NAME, nullptr, flags))
    {
        [[maybe_unused]] constexpr uint32_t PADDING_L_R = 20;

        // Set rounded-border child view
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

        if (ImGui::BeginChild(d_cconst('D', 'C', 'O', '1'), ImVec2(1600, 100 + ImGui::CalcTextSize("test").y * 2), true))
        {
            {
                int m_shape1 = int(fParamStorage[synthv1::DCO1_SHAPE1]);
                VeeOneWidgets::OscWave("DCO1 Wave 1", &fOscWave1, &m_shape1, &fParamStorage[synthv1::DCO1_WIDTH1], 80, 80);
                ImGui::SameLine(0, 20);

                _addWaveSelector("DCO1 Wave 1", synthv1::DCO1_SHAPE1); ImGui::SameLine(0, 20);
                _addKnob("Width 1", synthv1::DCO1_WIDTH1);
            }

            ImGui::SameLine(0, 40);

            {
                int m_shape2 = int(fParamStorage[synthv1::DCO1_SHAPE2]);
                VeeOneWidgets::OscWave("DCO1 Wave 2", &fOscWave2, &m_shape2, &fParamStorage[synthv1::DCO1_WIDTH2], 80, 80);
                ImGui::SameLine(0, 20);

                _addWaveSelector("DCO1 Wave 2", synthv1::DCO1_SHAPE2); ImGui::SameLine(0, 20);
                _addKnob("Width 2", synthv1::DCO1_WIDTH2); 
            }

            ImGui::SameLine(0, 40);

            _addKnob("Balance", synthv1::DCO1_BALANCE); ImGui::SameLine(0, 20);
            _addKnob("Detune", synthv1::DCO1_DETUNE); ImGui::SameLine(0, 20);
            _addKnob("Phase", synthv1::DCO1_PHASE); ImGui::SameLine(0, 40);

            _addKnob("Ring Mod", synthv1::DCO1_RINGMOD, true); ImGui::SameLine(0, 40);

            _addKnob("Octave", synthv1::DCO1_OCTAVE); ImGui::SameLine(0, 20);
            _addKnob("Tuning", synthv1::DCO1_TUNING); ImGui::SameLine(0, 40);

            _addKnob("Glide", synthv1::DCO1_GLIDE, true); ImGui::SameLine(0, 40);

            _addKnob("Env.Time", synthv1::DCO1_ENVTIME);

            
        }
        ImGui::EndChild();

        ImGui::Dummy(ImVec2(0, 10));    // Padding

        ImGui::BeginGroup();    // Group: DCF & LFO

        // TODO: Add DCF switch
        if (ImGui::BeginChild(d_cconst('D', 'C', 'F', '1'), ImVec2(540, 220), true))
        {
            //
            // Filter type
            //
            int dcf_type = int(fParamStorage[synthv1::DCF1_TYPE]);
            int dcf_slope = int(fParamStorage[synthv1::DCF1_SLOPE]);
            VeeOneWidgets::DCFCurve("DCF1", &fParamStorage[synthv1::DCF1_CUTOFF], &fParamStorage[synthv1::DCF1_RESO], &dcf_type, &dcf_slope, 220, 80);   
            ImGui::SameLine(0, 20);

            _addKnob("Cutoff", synthv1::DCF1_CUTOFF); ImGui::SameLine(0, 20);
            _addKnob("Resonance", synthv1::DCF1_RESO); ImGui::SameLine(0, 20);

            ImGui::BeginDisabled(VeeOneWidgets::FilterSlope(dcf_slope) == VeeOneWidgets::FilterSlope::s_formant);
            _addFilterTypeSelector("DCF1", synthv1::DCF1_TYPE); ImGui::SameLine(0, 20);
            ImGui::EndDisabled();
            
            _addFilterSlopeSelector("DCF1", synthv1::DCF1_SLOPE);

            //
            // Filter Envelope
            //
            _addKnob("Envelope", synthv1::DCF1_ENVELOPE); ImGui::SameLine(0, 10);
            VeeOneWidgets::ADSREnvelope("DCF1", &fParamStorage[synthv1::DCF1_ATTACK], &fParamStorage[synthv1::DCF1_DECAY], &fParamStorage[synthv1::DCF1_SUSTAIN], &fParamStorage[synthv1::DCF1_RELEASE], 160, 80);
            ImGui::SameLine(0, 20);

            _addKnob("Attack##DCF1", synthv1::DCF1_ATTACK); ImGui::SameLine(0, 20);
            _addKnob("Decay##DCF1", synthv1::DCF1_DECAY); ImGui::SameLine(0, 20);
            _addKnob("Sustain##DCF1", synthv1::DCF1_SUSTAIN); ImGui::SameLine(0, 20);
            _addKnob("Release##DCF1", synthv1::DCF1_RELEASE); ImGui::SameLine(0, 20);
        }
        ImGui::EndChild();

        ImGui::SameLine(0, 20);

        // TODO: Add LFO switch
        if (ImGui::BeginChild(d_cconst('L', 'F', 'O', '1'), ImVec2(720, 220), true))
        {
            {
                int m_shape = int(fParamStorage[synthv1::LFO1_SHAPE]);
                VeeOneWidgets::OscWave("LFO Wave 1", &fOscWave1, &m_shape, &fParamStorage[synthv1::LFO1_WIDTH], 80, 80);
                ImGui::SameLine(0, 20);

                _addWaveSelector("LFO Wave 1", synthv1::LFO1_SHAPE); ImGui::SameLine(0, 20);
                _addKnob("Width##LFO1", synthv1::LFO1_WIDTH);
            }

            ImGui::SameLine(0, 20);

            VeeOneWidgets::ADSREnvelope("LFO1", &fParamStorage[synthv1::LFO1_ATTACK], &fParamStorage[synthv1::LFO1_DECAY], &fParamStorage[synthv1::LFO1_SUSTAIN], &fParamStorage[synthv1::LFO1_RELEASE], 200, 80);
            ImGui::SameLine(0, 20);

            _addKnob("Attack##LFO1", synthv1::LFO1_ATTACK); ImGui::SameLine(0, 20);
            _addKnob("Decay##LFO1", synthv1::LFO1_DECAY); ImGui::SameLine(0, 20);
            _addKnob("Sustain##LFO1", synthv1::LFO1_SUSTAIN); ImGui::SameLine(0, 20);
            _addKnob("Release##LFO1", synthv1::LFO1_RELEASE);
            
            _addKnob("BPM", synthv1::LFO1_BPM); ImGui::SameLine(0, 20);
            _addKnob("Rate", synthv1::LFO1_RATE); ImGui::SameLine(0, 20);

            ImGui::BeginGroup();
            ImGui::Dummy(ImVec2(0, 40));
            _addCheckbox("Sync##LFO", synthv1::LFO1_SYNC);
            ImGui::EndGroup();

            ImGui::SameLine(0, 20);

            _addKnob("Sweep", synthv1::LFO1_SWEEP); ImGui::SameLine(0, 20);
            _addKnob("Pitch", synthv1::LFO1_PITCH); ImGui::SameLine(0, 20);
            _addKnob("Balance##LFO", synthv1::LFO1_BALANCE); ImGui::SameLine(0, 20);
            _addKnob("Ring Mod", synthv1::LFO1_RINGMOD); ImGui::SameLine(0, 20);
            _addKnob("Cutoff##LFO", synthv1::LFO1_CUTOFF); ImGui::SameLine(0, 20);
            _addKnob("Reso", synthv1::LFO1_RESO); ImGui::SameLine(0, 20);
            _addKnob("Panning##LFO", synthv1::LFO1_PANNING); ImGui::SameLine(0, 20);
            _addKnob("Volume##LFO", synthv1::LFO1_VOLUME); ImGui::SameLine(0, 20);                                                
        }
        ImGui::EndChild();

        ImGui::EndGroup();    // Group: DCF & LFO

        ImGui::Dummy(ImVec2(0, 10));    // Padding

        if (ImGui::BeginChild(d_cconst('D', 'C', 'A', '1'), ImVec2(540, 100 + ImGui::CalcTextSize("test").y * 2), true))
        {
            _addKnob("Volume##DCA", synthv1::DCA1_VOLUME); ImGui::SameLine(0, 10);
            VeeOneWidgets::ADSREnvelope("DCA1", &fParamStorage[synthv1::DCA1_ATTACK], &fParamStorage[synthv1::DCA1_DECAY], &fParamStorage[synthv1::DCA1_SUSTAIN], &fParamStorage[synthv1::DCA1_RELEASE], 160, 80);
            ImGui::SameLine(0, 20);

            _addKnob("Attack##DCA", synthv1::DCA1_ATTACK); ImGui::SameLine(0, 20);
            _addKnob("Decay##DCA", synthv1::DCA1_DECAY); ImGui::SameLine(0, 20);
            _addKnob("Sustain##DCA", synthv1::DCA1_SUSTAIN); ImGui::SameLine(0, 20);
            _addKnob("Release##DCA", synthv1::DCA1_RELEASE); ImGui::SameLine(0, 20);
        }
        ImGui::EndChild();

        ImGui::SameLine(0, 20);

        if (ImGui::BeginChild(d_cconst('D', 'E', 'F', '1'), ImVec2(440, 100 + ImGui::CalcTextSize("test").y * 2), true))
        {
            _addKnob("Pitchbend##DEF", synthv1::DEF1_PITCHBEND); ImGui::SameLine(0, 10);
            _addKnob("Modwheel##DEF", synthv1::DEF1_MODWHEEL); ImGui::SameLine(0, 20);
            _addKnob("Pressure##DEF", synthv1::DEF1_PRESSURE); ImGui::SameLine(0, 20);
            _addKnob("Velocity##DEF", synthv1::DEF1_VELOCITY); ImGui::SameLine(0, 20);
            _addKnobInt("Channel##DEF", synthv1::DEF1_CHANNEL); ImGui::SameLine(0, 20);

            _addKeyboardModeSelector("DEF1", synthv1::DEF1_MONO);
        }
        ImGui::EndChild();       

        ImGui::SameLine(0, 20);

        if (ImGui::BeginChild(d_cconst('O', 'U', 'T', '1'), ImVec2(260, 100 + ImGui::CalcTextSize("test").y * 2), true))
        {
            _addKnob("Width##Out", synthv1::OUT1_WIDTH); ImGui::SameLine(0, 10);
            _addKnob("Panning##Out", synthv1::OUT1_PANNING); ImGui::SameLine(0, 20);
            _addKnob("Effects##Out", synthv1::OUT1_FXSEND); ImGui::SameLine(0, 20);
            _addKnob("Volume##Out", synthv1::OUT1_VOLUME); ImGui::SameLine(0, 20);
        }
        ImGui::EndChild();  

        ImGui::PopStyleVar();   // ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    }
    ImGui::End();
}

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
	return new SynthV1PluginUI();
}

END_NAMESPACE_DISTRHO


//-------------------------------------------------------------------------
// synthv1_dpfui - Synth engine accessor: impl.
//

synthv1_dpfui::synthv1_dpfui(synthv1_dpf *pSynth, DISTRHO::SynthV1PluginUI *pluginUiInterface)
	: synthv1_ui(pSynth, true), m_plugin_ui(pluginUiInterface)
{
}

void synthv1_dpfui::write_function(synthv1::ParamIndex index, float fValue) const
{
	m_plugin_ui->setParameterValue(index, fValue);
}

// end of synthv1_dpfui.cpp
