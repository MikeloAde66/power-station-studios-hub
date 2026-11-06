// InnerGrooveLossProcessor.h
// Power Station Studios — Turntable module
//
// Models the natural high-frequency loss as the stylus tracks from the
// outer edge of a record toward the inner run-out groove (linear velocity
// drops toward the center, so highs compress/attenuate). Drives a
// first-order low-pass whose cutoff is swept by playback progress.
//
// Wire-up note: `progress` should come from the same transport position
// TurntableComponent.h uses for tonearm placement, so the visual and the
// DSP stay in sync. Not yet connected to that component.

#pragma once
#include <JuceHeader.h>

class InnerGrooveLossProcessor
{
public:
    void prepare (double sampleRate)
    {
        currentSampleRate = sampleRate;

        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = 512;
        spec.numChannels = 2;

        lowPassFilter.prepare (spec);
        updateCutoffFrequency (0.0f); // Start at outer groove
    }

    // progress ranges from 0.0 (outer groove) to 1.0 (inner groove)
    void updateCutoffFrequency (float progress)
    {
        progress = juce::jlimit (0.0f, 1.0f, progress);

        // Outer groove cutoff: ~20kHz (full bandwidth)
        // Inner groove cutoff: drops down to ~12kHz due to wavelength crowding
        float targetCutoff = juce::jmap (progress, 0.0f, 1.0f, 20000.0f, 12000.0f);

        auto coefficients = juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass (currentSampleRate, targetCutoff);
        *lowPassFilter.state = *coefficients;
    }

    void process (juce::AudioBuffer<float>& buffer)
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        lowPassFilter.process (context);
    }

private:
    double currentSampleRate = 44100.0;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowPassFilter;
};

// Usage, in the turntable engine's processBlock (not yet wired in):
//
//   float progress = static_cast<float>(transportSource.getCurrentPosition())
//                   / static_cast<float>(transportSource.getLengthInSeconds());
//   innerGrooveProcessor.updateCutoffFrequency(progress);
//   innerGrooveProcessor.process(buffer);
