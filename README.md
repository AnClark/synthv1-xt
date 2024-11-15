# synthv1-xt - new-generation port of synthv1, with a brand-new UI based on Dear ImGui

synthv1 is an old-school 4-oscillator subtractive polyphonic synthesizer with stereo fx. Written by [rncbc](https://github.com/rncbc) (a.k.a. Rui Nuno Capela).

Originally, synthv1 is for Linux only, providing a standalone JACK application, and a LV2 plugin. The latter is also available on Windows, but few DAWs support LV2 there (currently only REAPER, Ardour and MixBus).

This project aims at porting synthv1 to:

- more common platforms and plugin formats,
- and a brand-new GUI powerer by [Dear ImGui](https://github.com/ocornut/imgui),

with the great power of [DISTRHO Plugin Framework (DPF)](https://github.com/DISTRHO/DPF). I believe more musicians will enjoy this amazing synthesizer.

> **NOTE:** This project superceded the following projects:

- [My fork of synthv1 with DPF implementation](https://github.com/AnClark/synthv1/tree/dpf-implementation).
- [synthv1-universal](https://github.com/AnClark/synthv1-universal)

Any newer features and bugfixes will only be applied here.

## Features

- Cross-platform support
  - Windows
  - Linux
  - macOS _(to be tested)_
- Multi plugin formats
  - VST 2.4
  - VST3
  - LV2
  - CLAP
- New plugin UI based on Dear ImGui (WIP)

## Requirements

- Qt framework, C++ class library and tools for cross-platform application and UI development, <https://qt.io/>
- DISTRHO Plugin Framework, cross-platform unified audio plugin framework, <https://github.com/distrho/dpf>

## Roadmap

### Base UI implementation

- [ ] `Synth1` page
- [ ] `Effects` page
- [ ] `Synth2` page
- [ ] Topbar ([u-he](https://u-he.com/) flavor)
- [ ] `Randomize` button
- [ ] "About" window

### Presets

- [ ] Import/export preset
- [ ] Basic preset manager (like original synthv1)
- [ ] Advanced preset manager (with preset banks, categories, etc.)

### Extra features

- [ ] History queue (undo/redo support)
- [ ] Custom tuning
- [ ] "MIDI In" light
- [ ] Advanced controls for Compressor and Limiter FX


## Source code structure

- `plugin/`: The main implementation. This is the core of the project.
- `src/`: Official synthv1 repository. Serves as a Git submodule.
- `deps/`: External dependencies.
  - `dpf/`: DISTHRO Plugin Framework repository. Serves as a Git submodule.
  - `dpf-widgets/`: DPF's official 3rd-party widget compilation, including official port of Dear ImGui.
- `ui/`: (WIP) UI stuff, including custom widgets, window layouts, etc.
  - `veeone-widget-demo`: Dear ImGui widgets of graphs in official Vee-One plugins. Including OSC/LFO waveform view, ADSR view, and padthv1's sample waveform view. Written by AnClark Liu.

## How to build

### On Linux

#### 0. Install dependencies

Make sure you have the latest Qt 6.x installed. If not, install it. Refer to the document of your distribution.

```bash
# Ubuntu
sudo apt install qt6-base-dev

# Arch Linux
sudo pacman -S qt6-base
```

#### 1. Clone source code

This project has submodules, so make sure you don't omit `--recursive`:

```bash
git clone https://github.com/<to_be_named> synthv1 --recursive
```

If you forget, run the following command:

```bash
git submodule update --init --recursive
```

#### 2. Configure and build

```bash
cd synthv1
cmake -S . -B build
cmake --build build
```

#### 3. Run plugin

Built plugin resides in `synthv1/build/bin`. Copy them to you DAW's plugin scan paths.

## Q&A

### 1. Why do you choose DPF?

This project is based on DPF, for its multi-platform support. DPF provides a full solution for audio plugins, from base structures to final plugin binaries. You can build plugins for multi platforms and formats from just one source tree.

This project separates DPF port codes from official synthv1 repository, and uses its own CMake build rules. As synthv1 is in active development, this design helps keeping pace to rncbc's updates easily (by simply a `git pull` in `src/` directory).

### 2. Why not continue using Qt?

Sincerely speaking, Qt Widget is not a good toolkit for developing a cross-platform audio plugin.

#### Firstly, Qt has poor compatibility with DPF's external UI feature.

DPF itself supports 3rd-party UI toolkits like Qt, FLTK, NTK etc, as long as you manually create and manage UI instace yourself.

But actually doing such a management is a hard work. For example, I cannot make sure that the Qt Widget instance knows when to terminate, even though I tried to poll in `DISTRHO::UI::uiIdle()`. Chances are the plugin instance keeps on the backend even though the host (e.g. REAPER) has already exited.

So I migrated to Dear ImGui. Now DPF has official support for Dear ImGui toolkits. DPF itself provides a robust, full-featured management for OpenGL-based UIs, including Dear ImGui. So why not give it a try?

#### Secondly, Qt Widget behaves abnormally on Hi-DPI environment.

Qt itself has native Hi-DPI support, enabled seamlessly. But it usually works abnormally on embedded UI with DPF. For example, on my Windows 10 with 125% global scale:

- Window is clipped. There's always a part of window cannot be shown.
- Cannot resize window to get the whole window shown, since the rest of window contents continue to stretch as if the window is large enough. But the window is still clipped.

A workaround is to set an environment variable to explicitly disable Hi-DPI support. But this makes knobs shows abnormally.

Qt Widget is a good toolkit, but I think we'd better use it on standalone applications, rather than audio plugin UI.

#### Thirdly, Qt is too large.

For better portability, I link against static Qt on synthv1-universal. But Qt itself is too large, causing the resulted synthv1 binary becomes at least 40MB of size.

#### Summary

So, for better compatibilty and light-weight, I prefer Dear ImGui.

### 3. Do you want to "get rid of Qt"?

Sure. But not now.

By now (during ALPHA), I still use Qt Core and Qt XML, to make sure the DSP side works well with my new UI. Some of synthv1's core modules depends on Qt Core's features, like:

- `<QThread>, <QMutex>, <QWaitCondition>`: For `synthv1_sched`
- `<QHash>`: For manipulating key-value pairs within synthv1
- `<QString>` and other basic Qt library functions
- Qt XML: For managing presets

In future, I will migrate to other APIs and libraries, for example:

- Multi-threading: Prefer DPF's `Thread.hpp`, `Mutex.hpp`.
- QHash: Prefer `std::unordered_map`.
- XML access: Prefer [pugixml](https://pugixml.org/) or [MXML](https://www.msweet.org/mxml/).

## Notice

- Standalone JACK support is not provided in this project. Go to official repo if you need it.
- macOS should be supported on both Intel and Apple Silicon platform, as long as you have the right Qt 6.x development package installed (e.g. [by MacPorts](https://ports.macports.org/port/qt6/)). But I don't own a MacBook. Please help me test the plugin and give me a report.

## License

This project is licensed under GNU GPL v2.
