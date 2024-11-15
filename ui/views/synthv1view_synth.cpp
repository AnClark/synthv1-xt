// synthv1view_synth.cpp
//
/****************************************************************************
   Copyright (C) 2024, AnClark Liu. All rights reserved.

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
#include "veeone_adsr_widget.hpp"
#include "veeone_filter_widget.hpp"
#include "veeone_wave_widget.hpp"

// Color palettes.
static const auto SYNTH2_COLOR = IM_COL32(113, 166, 85, 255);
static const auto SYNTH2_COLOR_HOVERED = IM_COL32(162, 231, 143, 255);
static const auto SYNTH2_COLOR_ACTIVE = IM_COL32(116, 165, 102, 255);

void SynthV1PluginUI::_uiView_Synth(bool isSynth1)
{
    constexpr auto window_flags = ImGuiWindowFlags_MenuBar;

    // Set rounded-border child view
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

    // Use different color to distinguish the two synthesizers
    if (!isSynth1)  // synth 2
    {
        ImGui::PushStyleColor(ImGuiCol_Button, SYNTH2_COLOR);   // Used by ImGui::Button
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, SYNTH2_COLOR_HOVERED);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, SYNTH2_COLOR_ACTIVE);  // Used by ImGuiKnobs
        ImGui::PushStyleColor(ImGuiCol_CheckMark, SYNTH2_COLOR);
    }

    if (ImGui::BeginChild(d_cconst('D', 'C', 'O', '1'), ImVec2(1600, 100 + ImGui::CalcTextSize("test").y * 2), true, window_flags)) [[likely]]
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::Text(isSynth1 ? "DCO1" : "DCO2");
            ImGui::EndMenuBar();
        }

        {
            int m_shape1 = int(fParamStorage[isSynth1 ? synthv1::DCO1_SHAPE1 : synthv1::DCO2_SHAPE1]);
            VeeOneWidgets::OscWave("DCO Wave 1", &fOscWave1, &m_shape1, &fParamStorage[isSynth1 ? synthv1::DCO1_WIDTH1 : synthv1::DCO2_WIDTH1], 80, 80);
            ImGui::SameLine(0, 20);

            _addWaveSelector("DCO Wave 1", isSynth1 ? synthv1::DCO1_SHAPE1 : synthv1::DCO2_SHAPE1); ImGui::SameLine(0, 20);
            _addKnob("Width 1", isSynth1 ? synthv1::DCO1_WIDTH1 : synthv1::DCO2_WIDTH1);
        }

        ImGui::SameLine(0, 40);

        {
            int m_shape2 = int(fParamStorage[isSynth1 ? synthv1::DCO1_SHAPE2 : synthv1::DCO2_SHAPE2]);
            VeeOneWidgets::OscWave("DCO Wave 2", &fOscWave2, &m_shape2, &fParamStorage[isSynth1 ? synthv1::DCO1_WIDTH2 : synthv1::DCO2_WIDTH2], 80, 80);
            ImGui::SameLine(0, 20);

            _addWaveSelector("DCO Wave 2", isSynth1 ? synthv1::DCO1_SHAPE2 : synthv1::DCO2_SHAPE2); ImGui::SameLine(0, 20);
            _addKnob("Width 2", isSynth1 ? synthv1::DCO1_WIDTH2 : synthv1::DCO2_WIDTH2); 
        }

        ImGui::SameLine(0, 40);

        _addKnob("Balance", isSynth1 ? synthv1::DCO1_BALANCE : synthv1::DCO2_BALANCE); ImGui::SameLine(0, 20);
        _addKnob("Detune", isSynth1 ? synthv1::DCO1_DETUNE : synthv1::DCO2_DETUNE); ImGui::SameLine(0, 20);
        _addKnob("Phase", isSynth1 ? synthv1::DCO1_PHASE : synthv1::DCO2_PHASE); ImGui::SameLine(0, 40);

        _addKnob("Ring Mod", isSynth1 ? synthv1::DCO1_RINGMOD : synthv1::DCO2_RINGMOD, true); ImGui::SameLine(0, 40);

        _addKnob("Octave", isSynth1 ? synthv1::DCO1_OCTAVE : synthv1::DCO2_OCTAVE); ImGui::SameLine(0, 20);
        _addKnob("Tuning", isSynth1 ? synthv1::DCO1_TUNING : synthv1::DCO2_TUNING); ImGui::SameLine(0, 40);

        _addKnob("Glide", isSynth1 ? synthv1::DCO1_GLIDE : synthv1::DCO2_GLIDE, true); ImGui::SameLine(0, 40);

        _addKnob("Env.Time", isSynth1 ? synthv1::DCO1_ENVTIME : synthv1::DCO2_ENVTIME);

    }
    ImGui::EndChild();

    ImGui::Dummy(ImVec2(0, 10));    // Padding

    ImGui::BeginGroup();    // Group: DCF & LFO

    // TODO: Add DCF switch
    if (ImGui::BeginChild(d_cconst('D', 'C', 'F', '1'), ImVec2(540, 220), true, window_flags)) [[likely]]
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::Text(isSynth1 ? "DCF1" : "DCF2");

            ImGui::SameLine(540 - 50);
            _addToggle_NoLabel("##DCF Switch", isSynth1 ? synthv1::DCF1_ENABLED : synthv1::DCF2_ENABLED);

            ImGui::EndMenuBar();
        }

        ImGui::BeginDisabled(fParamStorage[isSynth1 ? synthv1::DCF1_ENABLED : synthv1::DCF2_ENABLED] <= 0.0f ? true : false);

        //
        // Filter type
        //
        int dcf_type = int(fParamStorage[isSynth1 ? synthv1::DCF1_TYPE : synthv1::DCF2_TYPE]);
        int dcf_slope = int(fParamStorage[isSynth1 ? synthv1::DCF1_SLOPE : synthv1::DCF2_SLOPE]);
        VeeOneWidgets::DCFCurve("DCF Curve", &fParamStorage[isSynth1 ? synthv1::DCF1_CUTOFF : synthv1::DCF2_CUTOFF], &fParamStorage[isSynth1 ? synthv1::DCF1_RESO : synthv1::DCF2_RESO], &dcf_type, &dcf_slope, 220, 80);   
        ImGui::SameLine(0, 20);

        _addKnob("Cutoff", isSynth1 ? synthv1::DCF1_CUTOFF : synthv1::DCF2_CUTOFF); ImGui::SameLine(0, 20);
        _addKnob("Resonance", isSynth1 ? synthv1::DCF1_RESO : synthv1::DCF2_RESO); ImGui::SameLine(0, 20);

        ImGui::BeginDisabled(VeeOneWidgets::FilterSlope(dcf_slope) == VeeOneWidgets::FilterSlope::s_formant);
        _addFilterTypeSelector("DCF", isSynth1 ? synthv1::DCF1_TYPE : synthv1::DCF2_TYPE); ImGui::SameLine(0, 20);
        ImGui::EndDisabled();
        
        _addFilterSlopeSelector("DCF", isSynth1 ? synthv1::DCF1_SLOPE : synthv1::DCF2_SLOPE);

        //
        // Filter Envelope
        //
        _addKnob("Envelope", isSynth1 ? synthv1::DCF1_ENVELOPE : synthv1::DCF2_ENVELOPE); ImGui::SameLine(0, 10);
        VeeOneWidgets::ADSREnvelope("DCF1", &fParamStorage[isSynth1 ? synthv1::DCF1_ATTACK : synthv1::DCF2_ATTACK], &fParamStorage[isSynth1 ? synthv1::DCF1_DECAY : synthv1::DCF2_DECAY], &fParamStorage[isSynth1 ? synthv1::DCF1_SUSTAIN : synthv1::DCF2_SUSTAIN], &fParamStorage[isSynth1 ? synthv1::DCF1_RELEASE : synthv1::DCF2_RELEASE], 160, 80);
        ImGui::SameLine(0, 20);

        _addKnob("Attack##DCF1", isSynth1 ? synthv1::DCF1_ATTACK : synthv1::DCF2_ATTACK); ImGui::SameLine(0, 20);
        _addKnob("Decay##DCF1", isSynth1 ? synthv1::DCF1_DECAY : synthv1::DCF2_DECAY); ImGui::SameLine(0, 20);
        _addKnob("Sustain##DCF1", isSynth1 ? synthv1::DCF1_SUSTAIN : synthv1::DCF2_SUSTAIN); ImGui::SameLine(0, 20);
        _addKnob("Release##DCF1", isSynth1 ? synthv1::DCF1_RELEASE : synthv1::DCF2_RELEASE); ImGui::SameLine(0, 20);

        ImGui::EndDisabled();   // ImGui::BeginDisabled(fParamStorage[isSynth1 ? synthv1::DCF1_ENABLED : synthv1::DCF2_ENABLED] <= 0.0f ? true : false);
    }
    ImGui::EndChild();

    ImGui::SameLine(0, 20);

    // TODO: Add LFO switch
    if (ImGui::BeginChild(d_cconst('L', 'F', 'O', '1'), ImVec2(720, 220), true, window_flags)) [[likely]]
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::Text(isSynth1 ? "LFO1" : "LFO2");

            ImGui::SameLine(720 - 50);
            _addToggle_NoLabel("##LFO Switch", isSynth1 ? synthv1::LFO1_ENABLED : synthv1::LFO2_ENABLED);

            ImGui::EndMenuBar();
        }

        ImGui::BeginDisabled(fParamStorage[isSynth1 ? synthv1::LFO1_ENABLED : synthv1::LFO2_ENABLED] <= 0.0f ? true : false);

        {
            int m_shape = int(fParamStorage[isSynth1 ? synthv1::LFO1_SHAPE : synthv1::LFO2_SHAPE]);
            VeeOneWidgets::OscWave("LFO Wave 1", &fLfoWave, &m_shape, &fParamStorage[isSynth1 ? synthv1::LFO1_WIDTH : synthv1::LFO2_WIDTH], 80, 80);
            ImGui::SameLine(0, 20);

            _addWaveSelector("LFO Wave 1", isSynth1 ? synthv1::LFO1_SHAPE : synthv1::LFO2_SHAPE); ImGui::SameLine(0, 20);
            _addKnob("Width##LFO1", isSynth1 ? synthv1::LFO1_WIDTH : synthv1::LFO2_WIDTH);
        }

        ImGui::SameLine(0, 20);

        VeeOneWidgets::ADSREnvelope("LFO Envelope", &fParamStorage[isSynth1 ? synthv1::LFO1_ATTACK : synthv1::LFO2_ATTACK], &fParamStorage[isSynth1 ? synthv1::LFO1_DECAY : synthv1::LFO2_DECAY], &fParamStorage[isSynth1 ? synthv1::LFO1_SUSTAIN : synthv1::LFO2_SUSTAIN], &fParamStorage[isSynth1 ? synthv1::LFO1_RELEASE : synthv1::LFO2_RELEASE], 200, 80);
        ImGui::SameLine(0, 20);

        _addKnob("Attack##LFO1", isSynth1 ? synthv1::LFO1_ATTACK : synthv1::LFO2_ATTACK); ImGui::SameLine(0, 20);
        _addKnob("Decay##LFO1", isSynth1 ? synthv1::LFO1_DECAY : synthv1::LFO2_DECAY); ImGui::SameLine(0, 20);
        _addKnob("Sustain##LFO1", isSynth1 ? synthv1::LFO1_SUSTAIN : synthv1::LFO2_SUSTAIN); ImGui::SameLine(0, 20);
        _addKnob("Release##LFO1", isSynth1 ? synthv1::LFO1_RELEASE : synthv1::LFO2_RELEASE);
        
        _addKnob("BPM", isSynth1 ? synthv1::LFO1_BPM : synthv1::LFO2_BPM); ImGui::SameLine(0, 20);
        _addKnob("Rate", isSynth1 ? synthv1::LFO1_RATE : synthv1::LFO2_RATE); ImGui::SameLine(0, 20);

        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(0, 40));
        _addCheckbox("Sync##LFO", isSynth1 ? synthv1::LFO1_SYNC : synthv1::LFO2_SYNC);
        ImGui::EndGroup();

        ImGui::SameLine(0, 20);

        _addKnob("Sweep##LFO", isSynth1 ? synthv1::LFO1_SWEEP : synthv1::LFO2_SWEEP); ImGui::SameLine(0, 20);
        _addKnob("Pitch##LFO", isSynth1 ? synthv1::LFO1_PITCH : synthv1::LFO2_PITCH); ImGui::SameLine(0, 20);
        _addKnob("Balance##LFO", isSynth1 ? synthv1::LFO1_BALANCE : synthv1::LFO2_BALANCE); ImGui::SameLine(0, 20);
        _addKnob("Ring Mod##LFO", isSynth1 ? synthv1::LFO1_RINGMOD : synthv1::LFO2_RINGMOD); ImGui::SameLine(0, 20);
        _addKnob("Cutoff##LFO", isSynth1 ? synthv1::LFO1_CUTOFF : synthv1::LFO2_CUTOFF); ImGui::SameLine(0, 20);
        _addKnob("Reso##LFO", isSynth1 ? synthv1::LFO1_RESO : synthv1::LFO2_RESO); ImGui::SameLine(0, 20);
        _addKnob("Panning##LFO", isSynth1 ? synthv1::LFO1_PANNING : synthv1::LFO2_PANNING); ImGui::SameLine(0, 20);
        _addKnob("Volume##LFO", isSynth1 ? synthv1::LFO1_VOLUME : synthv1::LFO2_VOLUME); ImGui::SameLine(0, 20);

        ImGui::EndDisabled();   // ImGui::BeginDisabled(fParamStorage[isSynth1 ? synthv1::LFO1_ENABLED : synthv1::LFO2_ENABLED] <= 0.0f ? true : false);                                                
    }
    ImGui::EndChild();

    ImGui::EndGroup();    // Group: DCF & LFO

    ImGui::Dummy(ImVec2(0, 10));    // Padding

    if (ImGui::BeginChild(d_cconst('D', 'C', 'A', '1'), ImVec2(540, 100 + ImGui::CalcTextSize("test").y * 2), true, window_flags)) [[likely]]
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::Text(isSynth1 ? "DCA1" : "DCA2");
            ImGui::EndMenuBar();
        }

        _addKnob("Volume##DCA", isSynth1 ? synthv1::DCA1_VOLUME : synthv1::DCA2_VOLUME); ImGui::SameLine(0, 10);
        VeeOneWidgets::ADSREnvelope("DCA1", &fParamStorage[isSynth1 ? synthv1::DCA1_ATTACK : synthv1::DCA2_ATTACK], &fParamStorage[isSynth1 ? synthv1::DCA1_DECAY : synthv1::DCA2_DECAY], &fParamStorage[isSynth1 ? synthv1::DCA1_SUSTAIN : synthv1::DCA2_SUSTAIN], &fParamStorage[isSynth1 ? synthv1::DCA1_RELEASE : synthv1::DCA2_RELEASE], 160, 80);
        ImGui::SameLine(0, 20);

        _addKnob("Attack##DCA", isSynth1 ? synthv1::DCA1_ATTACK : synthv1::DCA2_ATTACK); ImGui::SameLine(0, 20);
        _addKnob("Decay##DCA", isSynth1 ? synthv1::DCA1_DECAY : synthv1::DCA2_DECAY); ImGui::SameLine(0, 20);
        _addKnob("Sustain##DCA", isSynth1 ? synthv1::DCA1_SUSTAIN : synthv1::DCA2_SUSTAIN); ImGui::SameLine(0, 20);
        _addKnob("Release##DCA", isSynth1 ? synthv1::DCA1_RELEASE : synthv1::DCA2_RELEASE); ImGui::SameLine(0, 20);
    }
    ImGui::EndChild();

    ImGui::SameLine(0, 20);

    if (ImGui::BeginChild(d_cconst('D', 'E', 'F', '1'), ImVec2(440, 100 + ImGui::CalcTextSize("test").y * 2), true, window_flags)) [[likely]]
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::Text(isSynth1 ? "DEF1" : "DEF2");
            ImGui::EndMenuBar();
        }

        _addKnob("Pitchbend##DEF", isSynth1 ? synthv1::DEF1_PITCHBEND : synthv1::DEF2_PITCHBEND); ImGui::SameLine(0, 10);
        _addKnob("Modwheel##DEF", isSynth1 ? synthv1::DEF1_MODWHEEL : synthv1::DEF2_MODWHEEL); ImGui::SameLine(0, 20);
        _addKnob("Pressure##DEF", isSynth1 ? synthv1::DEF1_PRESSURE : synthv1::DEF2_PRESSURE); ImGui::SameLine(0, 20);
        _addKnob("Velocity##DEF", isSynth1 ? synthv1::DEF1_VELOCITY : synthv1::DEF2_VELOCITY); ImGui::SameLine(0, 20);
        _addKnobInt("Channel##DEF", isSynth1 ? synthv1::DEF1_CHANNEL : synthv1::DEF2_CHANNEL); ImGui::SameLine(0, 20);

        _addKeyboardModeSelector("DEF1", isSynth1 ? synthv1::DEF1_MONO : synthv1::DEF2_MONO);
    }
    ImGui::EndChild();       

    ImGui::SameLine(0, 20);

    if (ImGui::BeginChild(d_cconst('O', 'U', 'T', '1'), ImVec2(260, 100 + ImGui::CalcTextSize("test").y * 2), true, window_flags)) [[likely]]
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::Text(isSynth1 ? "OUT1" : "OUT2");
            ImGui::EndMenuBar();
        }

        _addKnob("Width##Out", isSynth1 ? synthv1::OUT1_WIDTH : synthv1::OUT2_WIDTH); ImGui::SameLine(0, 20);
        _addKnob("Panning##Out", isSynth1 ? synthv1::OUT1_PANNING : synthv1::OUT2_PANNING); ImGui::SameLine(0, 20);
        _addKnob("Effects##Out", isSynth1 ? synthv1::OUT1_FXSEND : synthv1::OUT2_FXSEND); ImGui::SameLine(0, 20);
        _addKnob("Volume##Out", isSynth1 ? synthv1::OUT1_VOLUME : synthv1::OUT2_VOLUME); ImGui::SameLine(0, 20);
    }
    ImGui::EndChild();  

    if (!isSynth1)  // synth 2
    {
        ImGui::PopStyleColor(4);  // Color style vars
    }

    ImGui::PopStyleVar();   // ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
}

// end of synthv1view_synth.cpp
