cmake_minimum_required(VERSION 3.22)

project(Arranger VERSION 0.0.1)

list(APPEND CMAKE_PREFIX_PATH "~/.local/lib/JUCE")

find_package(JUCE CONFIG REQUIRED)

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

        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
target_compile_features(Arranger PRIVATE cxx_std_20)
target_compile_options(Arranger  PRIVATE -Ofast)


