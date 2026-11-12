cmake_minimum_required(VERSION 3.22)

# Must be set BEFORE project(): Live 10 is Intel-only, so we need x86_64
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")

project(BitFuzz VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4          # same tag as TapeWorn
    GIT_SHALLOW ON)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(BitFuzz
    COMPANY_NAME "Bruno"
    PLUGIN_MANUFACTURER_CODE Brun   # same manufacturer code as TapeWorn
    PLUGIN_CODE Bfzz                # MUST differ from TapeWorn's Twrn
    FORMATS AU VST3
    PRODUCT_NAME "BitFuzz"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE)

target_sources(BitFuzz PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_compile_definitions(BitFuzz PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(BitFuzz
    PRIVATE juce::juce_audio_utils
    PUBLIC  juce::juce_recommended_config_flags
            juce::juce_recommended_warning_flags)
