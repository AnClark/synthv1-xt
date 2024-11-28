#pragma once

#include "extra/String.hpp"

#include <vector>

// Forward decls.
namespace DISTRHO
{
class SynthV1PluginUI;
}

// TODO:
// - Allow banks/presets with the same name, and distinguish them with suffix (e.g. "_1", "_2", etc.)

// ------------------------------------------------------------------
// synthv1_preset - Single preset file (just the same as official *.synthv1 format).

struct synthv1_preset
{
    String name;    // Preset name
    String presetXml;   // Preset XML file content
    uint32_t weight = 0;    // Weight for sort order in preset bank
};

// ------------------------------------------------------------------
// synthv1_preset - Single preset bank file (like Steinberg VST 2.4 FXB, or REAPER RPL).

struct synthv1_preset_bank
{
    String name;
    String author;
    std::vector<synthv1_preset> presets;

    String description;
    String copyright;

    // TODO: Add category support (Helm behavior),
    //       or otherwise add markers to distinguish category (Memorymoon behavior)?
    // TODO: Allow custom ordering (by weight).
};

// ------------------------------------------------------------------
// synthv1_preset_manager - Manages presets / banks from UI side.

class synthv1_preset_manager
{
private:
    SynthV1PluginUI *fUi;

    std::vector<synthv1_preset_bank> fBanks;

public:
    synthv1_preset_manager(SynthV1PluginUI *uiInstance);

    uint32_t loadPreset(const char* sPresetXml);
    String savePreset(const char* presetName);

    uint32_t loadBank(const char* sBankXml, synthv1_preset_bank& newBank);
    String saveBank(const synthv1_preset_bank& bank);
};
