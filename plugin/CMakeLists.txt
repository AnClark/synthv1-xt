# project (${SYNTHV1_SRC_DIR}/synthv1)

set (SYNTHV1_SRC_DIR "${PROJECT_SOURCE_DIR}/src/src")

include_directories (
    ${SYNTHV1_SRC_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories (
  ${Qt6Core_INCLUDE_DIRS}
  ${Qt6Xml_INCLUDE_DIRS}
)


set (CMAKE_AUTOUIC ON)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)

if (EXISTS ${SYNTHV1_SRC_DIR}/config.h)
  file (REMOVE ${SYNTHV1_SRC_DIR}/config.h)
endif ()
configure_file (${SYNTHV1_SRC_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
configure_file (config_dpf.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config_dpf.h)


set (HEADERS
  ${SYNTHV1_SRC_DIR}/synthv1.h
  ${SYNTHV1_SRC_DIR}/synthv1_config.h
  ${SYNTHV1_SRC_DIR}/synthv1_filter.h
  ${SYNTHV1_SRC_DIR}/synthv1_formant.h
  ${SYNTHV1_SRC_DIR}/synthv1_wave.h
  ${SYNTHV1_SRC_DIR}/synthv1_ramp.h
  ${SYNTHV1_SRC_DIR}/synthv1_list.h
  ${SYNTHV1_SRC_DIR}/synthv1_fx.h
  ${SYNTHV1_SRC_DIR}/synthv1_reverb.h
  synthv1_param.h       # Overridden by my modded version
  ${SYNTHV1_SRC_DIR}/synthv1_sched.h
  ${SYNTHV1_SRC_DIR}/synthv1_tuning.h
  ${SYNTHV1_SRC_DIR}/synthv1_programs.h
  ${SYNTHV1_SRC_DIR}/synthv1_controls.h
)

set (SOURCES
  ${SYNTHV1_SRC_DIR}/synthv1.cpp
  ${SYNTHV1_SRC_DIR}/synthv1_config.cpp
  ${SYNTHV1_SRC_DIR}/synthv1_formant.cpp
  ${SYNTHV1_SRC_DIR}/synthv1_wave.cpp
  synthv1_param.cpp     # Overridden by my modded version
  ${SYNTHV1_SRC_DIR}/synthv1_sched.cpp
  ${SYNTHV1_SRC_DIR}/synthv1_tuning.cpp
  ${SYNTHV1_SRC_DIR}/synthv1_programs.cpp
  ${SYNTHV1_SRC_DIR}/synthv1_controls.cpp
)


set (HEADERS_UI
  ${SYNTHV1_SRC_DIR}/synthv1_ui.h
)

set (SOURCES_UI
  ${SYNTHV1_SRC_DIR}/synthv1_ui.cpp
)


add_library (${PROJECT_NAME}_core STATIC
  ${HEADERS}
  ${SOURCES}
  ${SOURCES_UI}
)

set_target_properties (${PROJECT_NAME}_core    PROPERTIES CXX_STANDARD 17)

#
# Build DPF plugin(s).
#

set (DPF_PLUGIN_TYPES)
if (CONFIG_DPF_VST2)
  list (APPEND DPF_PLUGIN_TYPES "vst2")
endif ()
if (CONFIG_DPF_VST3)
  list (APPEND DPF_PLUGIN_TYPES "vst3")
endif ()
if (CONFIG_DPF_CLAP)
  list (APPEND DPF_PLUGIN_TYPES "clap")
endif ()
if (CONFIG_DPF_LV2)
  list (APPEND DPF_PLUGIN_TYPES "lv2")
endif ()


set (DPF_WIDGETS_DIR ${PROJECT_SOURCE_DIR}/deps/dpf-widgets)
set (VEEONE_WIDGETS_DIR ${PROJECT_SOURCE_DIR}/ui/veeone-widgets)

dpf_add_plugin (${PROJECT_NAME}
  TARGETS ${DPF_PLUGIN_TYPES} jack
  FILES_DSP
    synthv1_dpf.cpp
  FILES_UI
    synthv1_dpfui.cpp
    synthv1_dpfui_helpers.cpp
    ${DPF_WIDGETS_DIR}/opengl/DearImGui.cpp
    ${VEEONE_WIDGETS_DIR}/veeone_wave_widget.cpp
    ${VEEONE_WIDGETS_DIR}/veeone_filter_widget.cpp
    ${VEEONE_WIDGETS_DIR}/veeone_adsr_widget.cpp
  FILES_COMMON
    synthv1_param.cpp
)

# Set to newer C++ standard so I can benefit from optimized features (constexpr std::string::find(), etc.)
set_target_properties (${PROJECT_NAME}       PROPERTIES CXX_STANDARD 20)
set_target_properties (${PROJECT_NAME}-dsp   PROPERTIES CXX_STANDARD 20)
set_target_properties (${PROJECT_NAME}-ui    PROPERTIES CXX_STANDARD 20)


# Add custom widgets include dir stuff.
target_include_directories (${PROJECT_NAME} PUBLIC
  # DPF Widgets
  ${DPF_WIDGETS_DIR}/generic
  ${DPF_WIDGETS_DIR}/opengl

  # Vee-One widgets demo
  ${VEEONE_WIDGETS_DIR}
  ${DPF_WIDGETS_DIR}/opengl/DearImGui   # For imgui.h used by Vee-One Widgets
)


if (QT_VERSION VERSION_GREATER_EQUAL 6.0.0)
  target_compile_options (${PROJECT_NAME}_core    PRIVATE -fPIC)  # Enable PIC for plugin core
endif ()

# Add some debugger flags.
if (CONFIG_DEBUG AND UNIX AND NOT APPLE)
  set (CONFIG_DEBUG_OPTIONS -g -fsanitize=address -fno-omit-frame-pointer)
endif ()

target_link_libraries (${PROJECT_NAME}    PUBLIC
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Xml
  ${PROJECT_NAME}_core
)


#
# Set install target dirs and files.
#

set (DPF_BINARY_DIR ${PROJECT_BINARY_DIR}/bin)

if (UNIX AND NOT APPLE)
  if (CONFIG_PREFIX MATCHES $ENV{HOME})
    set (CONFIG_VSTDIR ${CONFIG_PREFIX}/.vst)
    set (CONFIG_VST3DIR ${CONFIG_PREFIX}/.vst3)
    set (CONFIG_CLAPDIR ${CONFIG_PREFIX}/.clap)
    set (CONFIG_LV2DIR ${CONFIG_PREFIX}/.lv2)
  else ()
    set (CONFIG_VSTDIR ${CONFIG_LIBDIR}/vst)
    set (CONFIG_VST3DIR ${CONFIG_LIBDIR}/vst3)
    set (CONFIG_CLAPDIR ${CONFIG_LIBDIR}/clap)
    set (CONFIG_LV2DIR ${CONFIG_LIBDIR}/lv2)
  endif ()

  set (DPF_VST2_BINARY ${DPF_BINARY_DIR}/${PROJECT_NAME}-vst2.so)
  set (DPF_CLAP_BINARY ${DPF_BINARY_DIR}/${PROJECT_NAME}.clap)
  set (DPF_LV2_BINARY ${DPF_BINARY_DIR}/${PROJECT_NAME}.lv2/${PROJECT_NAME}.so)
  dpf__determine_vst3_package_architecture(VST3_ARCH)
  set (DPF_VST3_BINARY ${DPF_BINARY_DIR}/${PROJECT_NAME}.vst3/Contents/${VST3_ARCH}-linux/${PROJECT_NAME}.so)

  set (VST2_SUFFIX .so)
  set (VST3_SUFFIX .so)
elseif (WIN32)
  set (CONFIG_VSTDIR ${CONFIG_WINDOWS_VST_PATH})
  set (CONFIG_VST3DIR ${CONFIG_WINDOWS_VST3_PATH})
  set (CONFIG_CLAPDIR ${CONFIG_WINDOWS_CLAP_PATH})
  set (CONFIG_LV2DIR ${CONFIG_WINDOWS_LV2_PATH})

  set (DPF_VST2_BINARY ${DPF_BINARY_DIR}/${PROJECT_NAME}-vst2.dll)
  set (DPF_CLAP_BINARY ${DPF_BINARY_DIR}/${PROJECT_NAME}.clap)
  set (DPF_LV2_BINARY ${DPF_BINARY_DIR}/${PROJECT_NAME}.lv2/${PROJECT_NAME}.dll)
  dpf__determine_vst3_package_architecture(VST3_ARCH)
  set (DPF_VST3_BINARY ${DPF_BINARY_DIR}/${PROJECT_NAME}.vst3/Contents/${VST3_ARCH}-win/${PROJECT_NAME}.vst3)

  set (VST2_SUFFIX .dll)
  set (VST3_SUFFIX .vst3)
endif ()


#
# Install plugin(s).
#

if (CONFIG_DPF_VST2)
  install (FILES ${DPF_VST2_BINARY}
    RENAME ${PROJECT_NAME}-vst2${VST2_SUFFIX}
    DESTINATION ${CONFIG_VSTDIR})
endif ()

if (CONFIG_DPF_VST3)
  install (FILES ${DPF_VST3_BINARY}
    RENAME ${PROJECT_NAME}${VST3_SUFFIX}
    DESTINATION ${CONFIG_VST3DIR}/${PROJECT_NAME}.vst3/Contents/${VST3_ARCH}-linux)
endif ()

if (CONFIG_DPF_CLAP)
  install (FILES ${DPF_CLAP_BINARY}
    RENAME ${PROJECT_NAME}.clap
    DESTINATION ${CONFIG_CLAPDIR})
endif ()

if (CONFIG_DPF_LV2)   # Explicitly separated from the default LV2 build
  install (FILES ${DPF_LV2_BINARY}
    ${DPF_BINARY_DIR}/${PROJECT_NAME}.lv2/manifest.ttl
    ${DPF_BINARY_DIR}/${PROJECT_NAME}.lv2/${PROJECT_NAME}.ttl
    DESTINATION ${CONFIG_LV2DIR}/${PROJECT_NAME}.lv2)
endif ()

