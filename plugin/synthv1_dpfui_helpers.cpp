// synthv1_dpfui_helpers.cpp
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
#include "synthv1_param.h"

#include "DearImGuiKnobs/imgui-knobs.h"
#include "veeone_filter_widget.hpp"

// ------------------------------------------------------------------
// UI Helpers

void SynthV1PluginUI::_addKnob(const char* label, synthv1::ParamIndex paramIndex, bool isZeroMeansOff)
{
    DISTRHO_SAFE_ASSERT_RETURN(paramIndex >= 0 && paramIndex < synthv1::NUM_PARAMS, )

    constexpr float VEEONE_KNOB_SIZE = 40.0f;

#if 1
    //const char* _format = isZeroMeansOff ? (fParamStorage[paramIndex] == 0 ? "Off" : NULL) : NULL;    // TODO: Does this has better performance? Keep it here.
    std::string _format = synthv1_param::paramDisplayImGui(paramIndex, fParamStorage[paramIndex], isZeroMeansOff);

    ImGui::BeginGroup();

    // TODO: Center the text properly.
    ImGui::PushItemWidth(VEEONE_KNOB_SIZE);
    ImGui::Text("%s", _trimAfterDoubleHash(std::string(label)).c_str());
    ImGui::PopItemWidth();

    if (ImGuiKnobs::Knob(label, &fParamStorage[paramIndex], synthv1_param::paramMinValue(paramIndex), synthv1_param::paramMaxValue(paramIndex),
                        0.0f, _format.c_str(), ImGuiKnobVariant_Tick, VEEONE_KNOB_SIZE, ImGuiKnobFlags_NoTitle | ImGuiKnobFlags_NoInput))
#else
    if (ImGuiKnobs::Knob(label, &fParamStorage[paramIndex], synthv1_param::paramMinValue(paramIndex), synthv1_param::paramMaxValue(paramIndex)))
#endif
    {
        if (ImGui::IsItemActivated())
            editParameter(paramIndex, true);
        
        setParameterValue(paramIndex, fParamStorage[paramIndex]);
    }

    if (ImGui::IsItemDeactivated())
    {
        editParameter(paramIndex, false);
    }

    // Origianlly, ImGuiKnobs::Knob provides a drag button to display knob value, and allows users to drag and input new values.
    // But it is not suitable for synthv1, since the scale of displayed value and actual value are differ.
    // To avoid misleading users, I replaced the drag button with a simple button.
    // TODO: Allow users to input new value via a pop-up box.
    ImGui::Button(_format.append("###").append(std::string(label)).c_str(), ImVec2(VEEONE_KNOB_SIZE, 0));

    ImGui::EndGroup();
}

void SynthV1PluginUI::_addKnobInt(const char* label, synthv1::ParamIndex paramIndex)
{
    DISTRHO_SAFE_ASSERT_RETURN(paramIndex >= 0 && paramIndex < synthv1::NUM_PARAMS, )

    constexpr float VEEONE_KNOB_SIZE = 40.0f;

    std::string _format = synthv1_param::paramDisplayImGui(paramIndex, fParamStorage[paramIndex], false);

    ImGui::BeginGroup();

    // TODO: Center the text properly.
    ImGui::PushItemWidth(VEEONE_KNOB_SIZE);
    ImGui::Text("%s", _trimAfterDoubleHash(std::string(label)).c_str());
    ImGui::PopItemWidth();

    int param_int_value = int(fParamStorage[paramIndex]);

    if (ImGuiKnobs::KnobInt(label, &param_int_value, synthv1_param::paramMinValue(paramIndex), synthv1_param::paramMaxValue(paramIndex),
                        0.0f, _format.c_str(), ImGuiKnobVariant_Tick, VEEONE_KNOB_SIZE, ImGuiKnobFlags_NoTitle | ImGuiKnobFlags_NoInput))
    {
        if (ImGui::IsItemActivated())
            editParameter(paramIndex, true);
        
        float new_value = synthv1_param::paramSafeValue(paramIndex, float(param_int_value));
        setParameterValue(paramIndex, new_value);
        parameterChanged(paramIndex, new_value);    // Trigger param storage update & repaint
    }

    if (ImGui::IsItemDeactivated())
    {
        editParameter(paramIndex, false);
    }

    // Origianlly, ImGuiKnobs::Knob provides a drag button to display knob value, and allows users to drag and input new values.
    // But it is not suitable for synthv1, since the scale of displayed value and actual value are differ.
    // To avoid misleading users, I replaced the drag button with a simple button.
    // TODO: Allow users to input new value via a pop-up box.
    ImGui::Button(_format.append("###").append(std::string(label)).c_str(), ImVec2(VEEONE_KNOB_SIZE, 0));

    ImGui::EndGroup();
}

void SynthV1PluginUI::_addCheckbox(const char* label, synthv1::ParamIndex paramIndex)
{
    bool value = fParamStorage[paramIndex] > 0 ? true : false;

    if (ImGui::Checkbox(label, &value))
    {
        if (ImGui::IsItemActivated())
            editParameter(paramIndex, true);

        setParameterValue(paramIndex, value ? 1.0f : 0.0f);
        parameterChanged(paramIndex, value ? 1.0f : 0.0f);    // Trigger param storage update & repaint
    }

    if (ImGui::IsItemDeactivated())
    {
        editParameter(paramIndex, false);
    }
}

void SynthV1PluginUI::_addToggle_NoLabel(const char* id, synthv1::ParamIndex paramIndex)
{
    bool value = fParamStorage[paramIndex] > 0 ? true : false;

    if (ImGui::Toggle(id, &value, ImVec2(0, 20)))
    {
        if (ImGui::IsItemActivated())
            editParameter(paramIndex, true);

        setParameterValue(paramIndex, value ? 1.0f : 0.0f);
        parameterChanged(paramIndex, value ? 1.0f : 0.0f);    // Trigger param storage update & repaint
    }

    if (ImGui::IsItemDeactivated())
    {
        editParameter(paramIndex, false);
    }
}

void SynthV1PluginUI::_addWaveSelector(const char* label, synthv1::ParamIndex paramIndex)
{
    DISTRHO_SAFE_ASSERT_RETURN(paramIndex == synthv1::DCO1_SHAPE1 || paramIndex == synthv1::DCO1_SHAPE2 
                                || paramIndex == synthv1::DCO2_SHAPE1 || paramIndex == synthv1::DCO2_SHAPE2
                                || paramIndex == synthv1::LFO1_SHAPE || paramIndex == synthv1::LFO2_SHAPE, )

    ImGui::BeginGroup();

    int waveform_value = int(fParamStorage[paramIndex]);

    bool s1 = ImGui::RadioButton(std::string("Pulse##").append(label).c_str(), &waveform_value, synthv1_wave_lf::Pulse);
    bool s2 = ImGui::RadioButton(std::string("Saw##").append(label).c_str(), &waveform_value, synthv1_wave_lf::Saw);
    bool s3 = ImGui::RadioButton(std::string("Sine##").append(label).c_str(), &waveform_value, synthv1_wave_lf::Sine);
    bool s4 = ImGui::RadioButton(std::string("Random##").append(label).c_str(), &waveform_value, synthv1_wave_lf::Rand);
    bool s5 = ImGui::RadioButton(std::string("Noise##").append(label).c_str(), &waveform_value, synthv1_wave_lf::Noise);

    if (s1 || s2 || s3 || s4 || s5)
    {
        if (ImGui::IsItemActivated())
            editParameter(paramIndex, true);

        float new_value = synthv1_param::paramSafeValue(paramIndex, float(waveform_value));
        setParameterValue(paramIndex, new_value);
        parameterChanged(paramIndex, new_value);    // Trigger param storage update & repaint
    }

    if (ImGui::IsItemDeactivated())
    {
        editParameter(paramIndex, false);
    }

    ImGui::EndGroup();
}

void SynthV1PluginUI::_addFilterTypeSelector(const char* label, synthv1::ParamIndex paramIndex)
{
    DISTRHO_SAFE_ASSERT_RETURN(paramIndex == synthv1::DCF1_TYPE || paramIndex == synthv1::DCF2_TYPE, )

    ImGui::BeginGroup();

    int filter_type_value = int(fParamStorage[paramIndex]);

    bool s1 = ImGui::RadioButton(std::string("LPF##").append(label).c_str(), &filter_type_value, (int)VeeOneWidgets::DcfType::t_LPF);
    bool s2 = ImGui::RadioButton(std::string("BPF##").append(label).c_str(), &filter_type_value, (int)VeeOneWidgets::DcfType::t_BPF);
    bool s3 = ImGui::RadioButton(std::string("HPF##").append(label).c_str(), &filter_type_value, (int)VeeOneWidgets::DcfType::t_HPF);
    bool s4 = ImGui::RadioButton(std::string("BRF##").append(label).c_str(), &filter_type_value, (int)VeeOneWidgets::DcfType::t_BRF);

    if (s1 || s2 || s3 || s4)
    {
        if (ImGui::IsItemActivated())
            editParameter(paramIndex, true);

        float new_value = synthv1_param::paramSafeValue(paramIndex, float(filter_type_value));
        setParameterValue(paramIndex, new_value);
        parameterChanged(paramIndex, new_value);    // Trigger param storage update & repaint
    }

    if (ImGui::IsItemDeactivated())
    {
        editParameter(paramIndex, false);
    }

    ImGui::EndGroup();
}

void SynthV1PluginUI::_addFilterSlopeSelector(const char* label, synthv1::ParamIndex paramIndex)
{
    DISTRHO_SAFE_ASSERT_RETURN(paramIndex == synthv1::DCF1_SLOPE || paramIndex == synthv1::DCF2_SLOPE, )

    ImGui::BeginGroup();

    int slope_type_value = int(fParamStorage[paramIndex]);

    bool s1 = ImGui::RadioButton(std::string("12dB/oct##").append(label).c_str(), &slope_type_value, (int)VeeOneWidgets::FilterSlope::s_12dbPerOct);
    bool s2 = ImGui::RadioButton(std::string("24dB/oct##").append(label).c_str(), &slope_type_value, (int)VeeOneWidgets::FilterSlope::s_24dBPerOct);
    bool s3 = ImGui::RadioButton(std::string("Biquad##").append(label).c_str(), &slope_type_value, (int)VeeOneWidgets::FilterSlope::s_biquad);
    bool s4 = ImGui::RadioButton(std::string("Formant##").append(label).c_str(), &slope_type_value, (int)VeeOneWidgets::FilterSlope::s_formant);

    if (s1 || s2 || s3 || s4)
    {
        if (ImGui::IsItemActivated())
            editParameter(paramIndex, true);

        float new_value = synthv1_param::paramSafeValue(paramIndex, float(slope_type_value));
        setParameterValue(paramIndex, new_value);
        parameterChanged(paramIndex, new_value);    // Trigger param storage update & repaint
    }

    if (ImGui::IsItemDeactivated())
    {
        editParameter(paramIndex, false);
    }

    ImGui::EndGroup();
}

void SynthV1PluginUI::_addKeyboardModeSelector(const char* label, synthv1::ParamIndex paramIndex)
{
    DISTRHO_SAFE_ASSERT_RETURN(paramIndex == synthv1::DEF1_MONO || paramIndex == synthv1::DEF2_MONO, )

    ImGui::BeginGroup();

    int keyboard_mode_value = int(fParamStorage[paramIndex]);

    bool s1 = ImGui::RadioButton(std::string("Poly##").append(label).c_str(), &keyboard_mode_value, 0);
    bool s2 = ImGui::RadioButton(std::string("Mono##").append(label).c_str(), &keyboard_mode_value, 1);
    bool s3 = ImGui::RadioButton(std::string("Legato##").append(label).c_str(), &keyboard_mode_value, 2);

    if (s1 || s2 || s3)
    {
        if (ImGui::IsItemActivated())
            editParameter(paramIndex, true);

        float new_value = synthv1_param::paramSafeValue(paramIndex, float(keyboard_mode_value));
        setParameterValue(paramIndex, new_value);
        parameterChanged(paramIndex, new_value);    // Trigger param storage update & repaint
    }

    if (ImGui::IsItemDeactivated())
    {
        editParameter(paramIndex, false);
    }

    ImGui::EndGroup();
}

void SynthV1PluginUI::_addViewSwitchButton(const char* label, View viewIndex)
{
    if (fCurrentView == viewIndex) {
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(7.0f, 0.8f, 0.8f));
    }

    if (ImGui::Button(label))
        fCurrentView = viewIndex;

    if (fCurrentView == viewIndex)
        ImGui::PopStyleColor(3);
}

// end of synthv1_dpfui_helpers.cpp
