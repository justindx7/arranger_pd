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
)


juce_generate_juce_header(Arranger)

file(GLOB HANDLER_FILES    
    "handlers/*.cpp"
)

target_sources(Arranger
        PRIVATE
        PluginEditor.cpp
        PluginProcessor.cpp
        ${HANDLER_FILES}
        )

        target_compile_definitions(Arranger
        PUBLIC
        JUCE_REPORT_APP_USAGE=0
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)


target_link_libraries(Arranger
        PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
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

find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)

find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
if (CMAKE_CXX_CPPCHECK)
    message("-- CPPCHECK FOUND")
    list(
        APPEND CMAKE_CXX_CPPCHECK 
            "--enable=warning"
            "--inconclusive"
            "--suppress=missingInclude"
            "--force" 
            "--inline-suppr"
            "--suppressions-list=${CMAKE_SOURCE_DIR}/CppCheckSupressions.txt"
            "--error-exitcode=1"
            "--verbose"  # Optional: adds more detailed output
    )
    add_custom_target(
        cppcheck
        COMMAND ${CMAKE_CXX_CPPCHECK}
            ${CMAKE_SOURCE_DIR}/PluginEditor.cpp
            ${CMAKE_SOURCE_DIR}/PluginProcessor.cpp
            ${CMAKE_SOURCE_DIR}/${HANDLER_FILES}
    )

endif()

add_dependencies(Arranger cppcheck)

