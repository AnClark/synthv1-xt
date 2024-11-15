// DistrhoPluginInfo.h
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

#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_BRAND "rncbc, AnClark Liu"
#define DISTRHO_PLUGIN_NAME "synthv1 XT"
#define DISTRHO_PLUGIN_URI "https://github.com/AnClark/synthv1-xt"
#define DISTRHO_PLUGIN_CLAP_ID "rncbc.synthv1"

#define DISTRHO_PLUGIN_HAS_UI 1
#define DISTRHO_PLUGIN_IS_RT_SAFE 1
#define DISTRHO_PLUGIN_IS_SYNTH 1
#define DISTRHO_PLUGIN_NUM_INPUTS 2
#define DISTRHO_PLUGIN_NUM_OUTPUTS 2
#define DISTRHO_UI_USER_RESIZABLE 1

/**
   Whether the %UI uses a custom toolkit implementation based on OpenGL.@n
   When enabled, the macros @ref DISTRHO_UI_CUSTOM_INCLUDE_PATH and @ref DISTRHO_UI_CUSTOM_WIDGET_TYPE are required.
 */
#define DISTRHO_UI_USE_CUSTOM 1

/**
   The include path to the header file used by the custom toolkit implementation.
   This path must be relative to dpf/distrho/DistrhoUI.hpp
   @see DISTRHO_UI_USE_CUSTOM
 */
#define DISTRHO_UI_CUSTOM_INCLUDE_PATH "DearImGui.hpp"

/**
   The top-level-widget typedef to use for the custom toolkit.
   This widget class MUST be a subclass of DGL TopLevelWindow class.
   It is recommended that you keep this widget class inside the DGL namespace,
   and define widget type as e.g. DGL_NAMESPACE::MyCustomTopLevelWidget.
   @see DISTRHO_UI_USE_CUSTOM
 */
#define DISTRHO_UI_CUSTOM_WIDGET_TYPE DGL_NAMESPACE::ImGuiTopLevelWidget


#define DISTRHO_UI_DEFAULT_WIDTH 1300
#define DISTRHO_UI_DEFAULT_HEIGHT 550


// Enable time position support (for measuring BPM)
#define DISTRHO_PLUGIN_WANT_TIMEPOS 1


// Allow UI to access DSP side directly.
// This is discouraged by DPF, but synthv1's official Qt UIindeed need this,
// because synthv1's Qt UI accesses synth engine, see synthv1widget.cpp.
//
// After migrating to DGL and Dear ImGui, synthv1 no longer need this.
// So I disabled it to keep track of DPF's policy.
#if 0
#define DISTRHO_PLUGIN_WANT_DIRECT_ACCESS 1
#endif

#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED

// end of DistrhoPluginInfo.h
