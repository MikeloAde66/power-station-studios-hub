// EightTrackPluginProcessor.h
// Power Station Studios — 8-Track module
//
// Wraps EightTrackCartridgeEngine.h into a real juce::AudioProcessor.
// As noted in EightTrackCartridgeEngine.h itself, programOffsets[] is
// computed but there's no actual audio source wired to seek on program
// change — this processor currently only passes audio through plus the
// solenoid click transient on switchProgram(). It does not yet play
// distinct audio per program.

#pragma once
#include <JuceHeader.h>
#include "EightTrackCartridgeEngine.h"

class EightTrackAudioProcessor : public juce::AudioProcessor
{
public:
    EightTrackAudioProcessor();
    ~EightTrackAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "8-Track"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override {}
    void setStateInformation (const void*, int) override {}

    void switchProgram() { engine.switchProgram(); }
    int getCurrentCartridgeProgram() const { return engine.getCurrentProgram(); }

private:
    EightTrackCartridgeEngine engine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EightTrackAudioProcessor)
};
