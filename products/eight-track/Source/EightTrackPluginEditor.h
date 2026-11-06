// EightTrackPluginEditor.h
// Power Station Studios — 8-Track module
//
// PLACEHOLDER UI. No cartridge/cartridge-slot visual design has been
// provided for this module yet (unlike Turntable, which has
// TurntableComponent.h). This is a plain button + label so the plugin
// has *an* editor and is testable, not a designed interface.

#pragma once
#include <JuceHeader.h>
#include "EightTrackPluginProcessor.h"

class EightTrackAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::Timer
{
public:
    explicit EightTrackAudioProcessorEditor (EightTrackAudioProcessor&);
    ~EightTrackAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    EightTrackAudioProcessor& audioProcessor;

    juce::TextButton switchProgramButton { "Switch Program" };
    juce::Label programLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EightTrackAudioProcessorEditor)
};
