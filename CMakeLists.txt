cmake_minimum_required(VERSION 3.22)

project(PowerStationStudios VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE)

# ---------------------------------------------------------------------------
# Power Station — the hub itself.
#
# ASSUMPTION (flag this if wrong): built as a Standalone app only, not a
# plugin. The hub is meant to host the other modules as graph nodes inside
# itself, so it reads as an app/rack rather than something you'd load inside
# another DAW. If you actually want Power Station loadable as a VST3/AU too
# (e.g. a rack-within-a-rack), add those formats below the same way
# PowerStationHub_AU / PowerStationHub_VST3 targets are added for the other
# modules.
# ---------------------------------------------------------------------------
juce_add_gui_app(PowerStationHub
    PRODUCT_NAME "Power Station Studios"
    COMPANY_NAME "Power Station Studios"
    BUNDLE_ID "com.powerstationstudios.hub"
)

target_sources(PowerStationHub PRIVATE
    hub/Source/Main.cpp
    hub/Source/PowerStationStudioHub.h
    hub/Source/GraphRoutingHelpers.h
    hub/Source/PatchCableComponent.h
    hub/Source/PatchJackComponent.h
    hub/Source/PatchBayMatrixComponent.h
)

target_compile_definitions(PowerStationHub PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:PowerStationHub,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:PowerStationHub,JUCE_VERSION>"
)

target_link_libraries(PowerStationHub PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# ---------------------------------------------------------------------------
# Turntable — standalone instrument plugin.
#
# ASSUMPTION: formats = VST3 + AU + Standalone, matching the pattern implied
# by ApexColor's build (Standalone .app in your screenshot) and the fact
# these are meant to sell as standard plugins. Confirm AU/VST3 vs. AUv3/AAX
# needs.
# ---------------------------------------------------------------------------
juce_add_plugin(Turntable
    PRODUCT_NAME "Turntable"
    COMPANY_NAME "Power Station Studios"
    BUNDLE_ID "com.powerstationstudios.turntable"
    FORMATS Standalone VST3 AU
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    PLUGIN_MANUFACTURER_CODE Pwst
    PLUGIN_CODE Ttbl
)

target_sources(Turntable PRIVATE
    products/turntable/Source/TurntablePluginProcessor.cpp
    products/turntable/Source/TurntablePluginProcessor.h
    products/turntable/Source/TurntablePluginEditor.cpp
    products/turntable/Source/TurntablePluginEditor.h
    products/turntable/Source/TurntableComponent.h
    products/turntable/Source/InnerGrooveLossProcessor.h
)

target_compile_definitions(Turntable PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

target_link_libraries(Turntable PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# ---------------------------------------------------------------------------
# 8-Track — standalone instrument plugin. Same assumptions as Turntable.
# ---------------------------------------------------------------------------
juce_add_plugin(EightTrack
    PRODUCT_NAME "8-Track"
    COMPANY_NAME "Power Station Studios"
    BUNDLE_ID "com.powerstationstudios.eighttrack"
    FORMATS Standalone VST3 AU
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    PLUGIN_MANUFACTURER_CODE Pwst
    PLUGIN_CODE EtTk
)

target_sources(EightTrack PRIVATE
    products/eight-track/Source/EightTrackPluginProcessor.cpp
    products/eight-track/Source/EightTrackPluginProcessor.h
    products/eight-track/Source/EightTrackPluginEditor.cpp
    products/eight-track/Source/EightTrackPluginEditor.h
    products/eight-track/Source/EightTrackCartridgeEngine.h
)

target_compile_definitions(EightTrack PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

target_link_libraries(EightTrack PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
