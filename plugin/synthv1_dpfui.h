// synthv1_dpfui.h
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

#ifndef __synthv1_dpfui_h
#define __synthv1_dpfui_h

#include "DistrhoUI.hpp"
#include "synthv1_preset.h"
#include "synthv1_ui.h"

#include "synthv1_wave.h"

#include "ResizeHandle.hpp"

// Forward decls.
class synthv1_dpf;
class synthv1_wave_lf;

// -----------------------------------------------------------------------------------------------------------
// SynthV1PluginUI - DPF Plugin UI interface.

START_NAMESPACE_DISTRHO

class SynthV1PluginUI : public UI
{
public:
    SynthV1PluginUI();

protected:
    void parameterChanged(uint32_t index, float value) override;
    void onImGuiDisplay() override;

private:
    float fParamStorage[synthv1::NUM_PARAMS];   // Store param values locally for operating with Dear ImGui widgets
    ResizeHandle fResizeHandle;

    String fConfigDir;

    std::unique_ptr<synthv1_preset_manager> fPresetManager;
    friend synthv1_preset_manager;

    synthv1_wave_lf fOscWave1, fOscWave2, fLfoWave;

    void _addKnob(const char* label, synthv1::ParamIndex paramIndex, bool isZeroMeansOff = false);   // Basic knob
    void _addKnobInt(const char* label, synthv1::ParamIndex paramIndex);
    void _addCheckbox(const char* label, synthv1::ParamIndex paramIndex);
    void _addToggle_NoLabel(const char* id, synthv1::ParamIndex paramIndex);

    void _addWaveSelector(const char* label, synthv1::ParamIndex paramIndex);
    void _addFilterTypeSelector(const char* label, synthv1::ParamIndex paramIndex);
    void _addFilterSlopeSelector(const char* label, synthv1::ParamIndex paramIndex);
    void _addKeyboardModeSelector(const char* label, synthv1::ParamIndex paramIndex);

    enum class View
    {
        Synth1 = 0,
        Synth2,
        Effects
    };
    View fCurrentView;
    void _addViewSwitchButton(const char* label, View viewIndex);

    void _uiView_Synth(bool isSynth1 = true);    // true: Synth 1; false: Synth 2
    void _uiView_Effects();

    // -----------------------------------------------------
    // Utils

    // Trim text after "##". This does the same behavior as ImGui::CalcTextSize().
    inline std::string _trimAfterDoubleHash(const std::string& str) {
        size_t pos = str.find("##");
        if (pos != std::string::npos) {
            return str.substr(0, pos);
        }
        return str;
    }

    void _randomParams();
    void _panic();

    void _actionBeforeLoadingPreset();
    void _actionAfterLoadingPreset();
    void _requestStabilize();


    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthV1PluginUI)
};

END_NAMESPACE_DISTRHO

//-------------------------------------------------------------------------
// synthv1_dpfui - Synth engine accessor: decl.
//

class synthv1_dpfui : public synthv1_ui
{
public:

	// Constructor.
	synthv1_dpfui(synthv1_dpf *pSynth, DISTRHO::SynthV1PluginUI *pluginUiInterface);

	// Accessors.
	void write_function(synthv1::ParamIndex index, float fValue) const;

private:
	DISTRHO::SynthV1PluginUI *m_plugin_ui;
};

//-------------------------------------------------------------------------
// Global functions.
//

// Get system-defined configuration directory.
DISTRHO::String get_system_config_dir();

#endif// __synthv1_dpfui_h

// end of synthv1_dpfui.h
