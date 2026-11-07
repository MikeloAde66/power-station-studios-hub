// EightTrackCartridgeEngine.h
// Power Station Studios — 8-Track module
//
// Models the 4-program interleaved structure of a 70s 8-track cartridge
// (Tracks 1-2 / 3-4 / 5-6 / 7-8 as four concurrent stereo programs) and
// the mechanical solenoid "click" transient when the head shifts program.
//
// Known gaps to resolve before this is build-ready:
//   - programOffsets[] is computed but never used in processBlock() —
//     there's no actual audio source/seek wired to the current program
//     yet, only the click transient.
//   - Cross-talk bleed between adjacent stereo pairs (-36 dB, mentioned
//     in the blueprint) and tape-wear high-frequency roll-off are not
//     implemented here yet.

#pragma once
#include <JuceHeader.h>

class EightTrackCartridgeEngine
{
public:
    void prepare (double sampleRate)
    {
        currentSampleRate = sampleRate;
        currentProgram = 0;

        // Define loop block offsets for the 4 physical programs (Programs 1 to 4)
        for (int i = 0; i < 4; ++i)
            programOffsets[i] = i * 30.0 * sampleRate;
    }

    void switchProgram()
    {
        currentProgram = (currentProgram + 1) % 4;
        triggerSolenoidTransient();
    }

    // Jump directly to a program (1-4), e.g. from a numbered selector
    // button rather than cycling through switchProgram(). Still fires
    // the same solenoid click transient as a real head-shift would.
    void setProgram (int program1to4)
    {
        int target = juce::jlimit (1, 4, program1to4) - 1;
        if (target == currentProgram)
            return;
        currentProgram = target;
        triggerSolenoidTransient();
    }

    void processBlock (juce::AudioBuffer<float>& buffer)
    {
        // Inject physical mechanical solenoid click thud transient on program change
        if (clickSamplesRemaining > 0)
        {
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                float clickSample = (random.nextFloat() * 2.0f - 1.0f) * 0.25f * (static_cast<float>(clickSamplesRemaining) / 120.0f);
                for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                {
                    buffer.addSample (ch, i, clickSample);
                }
                clickSamplesRemaining--;
                if (clickSamplesRemaining <= 0) break;
            }
        }
    }

    int getCurrentProgram() const { return currentProgram + 1; }

private:
    void triggerSolenoidTransient()
    {
        clickSamplesRemaining = 120; // ~2.7ms mechanical impulse at 44.1kHz
    }

    int currentProgram = 0;
    double currentSampleRate = 44100.0;
    int64_t programOffsets[4] = {0, 0, 0, 0};
    int clickSamplesRemaining = 0;
    juce::Random random;
};
