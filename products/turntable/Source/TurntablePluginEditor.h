// TurntablePluginEditor.h
// Power Station Studios — Turntable module
//
// Hosts TurntableComponent.h (platter/tonearm rendering) as the plugin's
// UI, plus a bare Play/Stop button — no 33/45 speed switch, no album-art
// loading UI, no noise-preset controls yet. Those are the remaining UI
// gaps called out in products/turntable/README.md.

#pragma once
#include <JuceHeader.h>
#include "TurntablePluginProcessor.h"
#include "TurntableComponent.h"

class TurntableAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       private juce::Timer
{
public:
    explicit TurntableAudioProcessorEditor (TurntableAudioProcessor&);
    ~TurntableAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    TurntableAudioProcessor& audioProcessor;

    TurntableComponent turntableView;
    juce::TextButton playStopButton { "Play" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TurntableAudioProcessorEditor)
};
