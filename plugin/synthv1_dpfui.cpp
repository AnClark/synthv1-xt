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

#include "synthv1.h"
#include "synthv1_dpf.h"

#include "hack_font.hpp"
#include "droidsans.hpp"

//-------------------------------------------------------------------------
// SynthV1PluginUI - DPF Plugin UI interface.
//

SynthV1PluginUI::SynthV1PluginUI()
    : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true),
      fResizeHandle(this)
{
    setGeometryConstraints(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true);

    // Load our own font
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImFontConfig _font_config_main;
    _font_config_main.GlyphMaxAdvanceX = 7.0f;  // Make the Hack font more condensed
    fFontMain = io.Fonts->AddFontFromMemoryCompressedTTF(Hack_Regular_compressed_data, Hack_Regular_compressed_size, 14.0f, &_font_config_main);

    fFontRadioButton = io.Fonts->AddFontFromMemoryCompressedTTF(DroidSans_compressed_data, DroidSans_compressed_size, 12.0f);
    fFontKnobValue = io.Fonts->AddFontFromMemoryCompressedTTF(DroidSans_compressed_data, DroidSans_compressed_size, 14.0f);

    // hide handle if UI is resizable
    if (isResizable())
        fResizeHandle.hide();

    // Set default params
    fCurrentView = View::Synth1;    // Default UI view

    // Obtain plugin config dir
    fConfigDir = get_system_config_dir() + "/synthv1";
    d_stderr("Config dir: %s", fConfigDir.buffer());

    // Initialize preset manager
    fPresetManager = std::make_unique<synthv1_preset_manager>(this);
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

        static constexpr ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        
        ImGui::BeginGroup();
        {
            if (ImGui::Button("Panic"))
            {
                _panic();
            }
            ImGui::SameLine(0, 40);

            if (ImGui::Button("Random"))
            {
                _randomParams();
            }
            ImGui::SameLine(0, 40);

            _addViewSwitchButton("Synth 1", View::Synth1); ImGui::SameLine();
            _addViewSwitchButton("Synth 2", View::Synth2); ImGui::SameLine();
            _addViewSwitchButton("Effects", View::Effects);
        }
        ImGui::EndGroup();

        ImGui::Separator();

        switch (fCurrentView)
        {
            case View::Synth1:
                _uiView_Synth(true);    // true means Synth 1
                break;
            
            case View::Synth2:
                _uiView_Synth(false);   // false means Synth 2
                break;
            
            case View::Effects:
                _uiView_Effects();
                break;
        }
    }
    ImGui::End();
}

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

START_NAMESPACE_DISTRHO

UI* createUI()
{
	return new SynthV1PluginUI();
}

END_NAMESPACE_DISTRHO


//-------------------------------------------------------------------------
// synthv1_dpfui - Synth engine accessor: impl.
//

synthv1_dpfui::synthv1_dpfui(synthv1_dpf *pSynth, SynthV1PluginUI *pluginUiInterface)
	: synthv1_ui(pSynth, true), m_plugin_ui(pluginUiInterface)
{
}

void synthv1_dpfui::write_function(synthv1::ParamIndex index, float fValue) const
{
	m_plugin_ui->setParameterValue(index, fValue);
}

// end of synthv1_dpfui.cpp
