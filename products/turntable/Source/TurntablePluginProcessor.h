// TurntablePluginProcessor.h
// Power Station Studios — Turntable module
//
// Wraps InnerGrooveLossProcessor.h into a real juce::AudioProcessor so the
// Turntable CMake target has something to instantiate. Does NOT yet load
// or play back an actual audio file — there is no transport/file-reading
// source wired in, so `progress` below is a placeholder that just creeps
// forward over time rather than tracking real playback position. That's
// the next gap to close, not something this file solves.

#pragma once
#include <JuceHeader.h>
#include "InnerGrooveLossProcessor.h"

class TurntableAudioProcessor : public juce::AudioProcessor
{
public:
    TurntableAudioProcessor();
    ~TurntableAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Turntable"; }
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

    // Exposed so the editor can drive play/stop and read progress for the
    // tonearm/platter UI. This is a stand-in for real transport state.
    void setPlaying (bool shouldPlay) { isPlaying = shouldPlay; }
    bool getPlaying() const { return isPlaying; }
    float getProgress() const { return progress; }

    void setSpeedRpm (float rpm) { currentRpm = rpm; }
    float getSpeedRpm() const { return currentRpm; }

private:
    InnerGrooveLossProcessor innerGrooveProcessor;
    bool isPlaying = false;
    float progress = 0.0f; // 0 = outer groove, 1 = inner groove; placeholder ramp
    float currentRpm = 33.333f;
    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TurntableAudioProcessor)
};
