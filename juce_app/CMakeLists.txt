cmake_minimum_required(VERSION 3.22)

project(Arranger VERSION 0.0.1)

# Look for JUCE in the installed location
list(APPEND CMAKE_PREFIX_PATH "~/.local/lib/JUCE")


find_path(JUCE_ROOT_DIR "~/.local/lib/JUCE")
find_package(JUCE CONFIG REQUIRED)

message("JUCE found at: ${JUCE_DIR}")
if(JUCE_VERSION)
    message("JUCE version: ${JUCE_VERSION}")
endif()

# Debug: List all available targets
get_property(imported_targets DIRECTORY PROPERTY IMPORTED_TARGETS)
message("Available imported targets: ${imported_targets}")

juce_add_module(/Users/justin/.local/lib/JUCE/include/JUCE-8.0.3/modules/juce_animation)

juce_add_plugin(Arranger
        VERSION 0.0.1
        COMPANY_NAME Application
        PLUGIN_MANUFACTURER_CODE Nord
        PLUGIN_CODE Arr4
        FORMATS Standalone
        PRODUCT_NAME "Arranger"
        IS_SYNTH TRUE
)

juce_generate_juce_header(Arranger)

file(GLOB HANDLER_FILES "handlers/*.cpp")
file(GLOB GUI_FILES "gui/*.cpp")
file(GLOB ARRANGER_FILES "arranger/*.cpp")
file(GLOB SERVICE_FILES "service/*.cpp")

set(ALL_SOURCE_FILES
    PluginEditor.cpp
    PluginProcessor.cpp
    ${HANDLER_FILES}
    ${GUI_FILES}
    ${ARRANGER_FILES}
    ${SERVICE_FILES}
)

target_sources(Arranger
        PRIVATE
        ${ALL_SOURCE_FILES}
)

target_compile_definitions(Arranger
        PUBLIC
        JUCE_REPORT_APP_USAGE=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Arranger
        PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_audio_formats
        juce::juce_gui_basics
        juce_animation
        juce::juce_opengl

        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
target_compile_features(Arranger PRIVATE cxx_std_20)
target_compile_options(Arranger  PRIVATE -Ofast)


