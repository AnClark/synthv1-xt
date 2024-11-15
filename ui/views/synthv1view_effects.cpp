// synthv1view_effects.cpp
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

constexpr auto TABLE_WIDTH_LEFT = 360;
constexpr auto TABLE_WIDTH_RIGHT = 270;

void SynthV1PluginUI::_uiView_Effects()
{
    constexpr auto window_flags = ImGuiWindowFlags_MenuBar;

    // Set rounded-border child view
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

    // Left padding
    // TODO: Auto setting padder's size
    ImGui::Dummy(ImVec2(200, 0));
    ImGui::SameLine();

    ImGui::BeginGroup();    // Main area

    if (ImGui::BeginChild(d_cconst('C', 'H', 'R', 'S'), ImVec2(TABLE_WIDTH_LEFT, 100 + ImGui::CalcTextSize("test").y * 2), true, window_flags)) [[likely]]
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::Text("Chorus");
            ImGui::EndMenuBar();
        }

        _addKnob("Wet##Chorus", synthv1::CHO1_WET, true); ImGui::SameLine(0, 40);
        _addKnob("Delay##Chorus", synthv1::CHO1_DELAY); ImGui::SameLine(0, 20);
        _addKnob("Feedback##Chorus", synthv1::CHO1_FEEDB); ImGui::SameLine(0, 20);
        _addKnob("Rate##Chorus", synthv1::CHO1_RATE); ImGui::SameLine(0, 20);
        _addKnob("Mod##Chorus", synthv1::CHO1_MOD);

        ImGui::EndChild();
    }

    ImGui::SameLine(0, 10);

    if (ImGui::BeginChild(d_cconst('F', 'L', 'G', 'R'), ImVec2(TABLE_WIDTH_RIGHT, 100 + ImGui::CalcTextSize("test").y * 2), true, window_flags)) [[likely]]
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::Text("Flanger");
            ImGui::EndMenuBar();
        }

        _addKnob("Wet##Flanger", synthv1::FLA1_WET, true); ImGui::SameLine(0, 40);
        _addKnob("Delay##Flanger", synthv1::FLA1_DELAY); ImGui::SameLine(0, 20);
        _addKnob("Feedback##Flanger", synthv1::FLA1_FEEDB); ImGui::SameLine(0, 20);
        _addKnob("Daft##Flanger", synthv1::FLA1_DAFT);

        ImGui::EndChild();
    }

    ImGui::Dummy(ImVec2(0, 10));

    if (ImGui::BeginChild(d_cconst('P', 'H', 'S', 'R'), ImVec2(TABLE_WIDTH_LEFT, 100 + ImGui::CalcTextSize("test").y * 2), true, window_flags)) [[likely]]
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::Text("Phaser");
            ImGui::EndMenuBar();
        }

        _addKnob("Wet##Phaser", synthv1::PHA1_WET, true); ImGui::SameLine(0, 40);
        _addKnob("Rate##Phaser", synthv1::PHA1_RATE); ImGui::SameLine(0, 20);
        _addKnob("Feedback##Phaser", synthv1::PHA1_FEEDB); ImGui::SameLine(0, 20);
        _addKnob("Depth##Phaser", synthv1::PHA1_DEPTH); ImGui::SameLine(0, 20);
        _addKnob("Daft##Phaser", synthv1::PHA1_DAFT);

        ImGui::EndChild();
    }

    ImGui::SameLine(0, 10);

    if (ImGui::BeginChild(d_cconst('D', 'L', 'A', 'Y'), ImVec2(TABLE_WIDTH_RIGHT, 100 + ImGui::CalcTextSize("test").y * 2), true, window_flags)) [[likely]]
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::Text("Delay");
            ImGui::EndMenuBar();
        }

        _addKnob("Wet##Delay", synthv1::DEL1_WET, true); ImGui::SameLine(0, 40);
        _addKnob("Delay##Delay", synthv1::DEL1_DELAY); ImGui::SameLine(0, 20);
        _addKnob("Feedback##Delay", synthv1::DEL1_FEEDB); ImGui::SameLine(0, 20);
        _addKnob("BPM##Delay", synthv1::DEL1_BPM);

        ImGui::EndChild();
    }

    ImGui::Dummy(ImVec2(0, 10));

    if (ImGui::BeginChild(d_cconst('R', 'V', 'R', 'B'), ImVec2(TABLE_WIDTH_LEFT, 100 + ImGui::CalcTextSize("test").y * 2), true, window_flags)) [[likely]]
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::Text("Reverb");
            ImGui::EndMenuBar();
        }

        _addKnob("Wet##Reverb", synthv1::REV1_WET, true); ImGui::SameLine(0, 40);
        _addKnob("Room##Reverb", synthv1::REV1_ROOM); ImGui::SameLine(0, 20);
        _addKnob("Damp##Reverb", synthv1::REV1_DAMP); ImGui::SameLine(0, 20);
        _addKnob("Feedback##Reverb", synthv1::REV1_FEEDB); ImGui::SameLine(0, 20);
        _addKnob("Width##Reverb", synthv1::REV1_WIDTH);

        ImGui::EndChild();
    }

    ImGui::SameLine(0, 10);

    if (ImGui::BeginChild(d_cconst('D', 'Y', 'N', '1'), ImVec2(TABLE_WIDTH_RIGHT, 100 + ImGui::CalcTextSize("test").y * 2), true, window_flags)) [[likely]]
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::Text("Dynamic");
            ImGui::EndMenuBar();
        }

        ImGui::Dummy(ImVec2(0, 50));

        _addCheckbox("Compressor", synthv1::DYN1_COMPRESS); ImGui::SameLine(0, 40);
        _addCheckbox("Limiter", synthv1::DYN1_LIMITER);

        ImGui::EndChild();
    }

    ImGui::EndGroup();  // Main area

    ImGui::PopStyleVar();   // ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
}

// end of synthv1view_effects.cpp
