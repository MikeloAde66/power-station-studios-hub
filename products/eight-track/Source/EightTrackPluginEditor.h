// EightTrackPluginEditor.h
// Power Station Studios — 8-Track module
//
// Hosts EightTrackComponent.h (wood chassis, cartridge slot, VU meters,
// transport buttons) as the plugin's UI. Replaces the earlier bare
// button + label placeholder.

#pragma once
#include <JuceHeader.h>
#include "EightTrackPluginProcessor.h"
#include "EightTrackComponent.h"
#include "EightTrackSkinLookAndFeel.h"

class EightTrackAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::Timer
{
public:
    explicit EightTrackAudioProcessorEditor (EightTrackAudioProcessor&);
    ~EightTrackAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    EightTrackAudioProcessor& audioProcessor;
    EightTrackSkinLookAndFeel eightTrackSkin;
    EightTrackComponent cartridgeView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EightTrackAudioProcessorEditor)
};
