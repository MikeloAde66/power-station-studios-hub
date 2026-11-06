#include "TurntablePluginProcessor.h"
#include "TurntablePluginEditor.h"

TurntableAudioProcessor::TurntableAudioProcessor()
    : AudioProcessor (BusesProperties()
                           .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                           .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

void TurntableAudioProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = sampleRate;
    innerGrooveProcessor.prepare (sampleRate);
}

void TurntableAudioProcessor::releaseResources()
{
}

bool TurntableAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo();
}

void TurntableAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    if (isPlaying)
    {
        // Placeholder progress ramp (~3 minutes outer-to-inner at 44.1kHz).
        // Replace with real transport position once a file-playback source
        // exists.
        progress = juce::jlimit (0.0f, 1.0f,
            progress + static_cast<float> (buffer.getNumSamples() / (currentSampleRate * 180.0)));
    }

    innerGrooveProcessor.updateCutoffFrequency (progress);
    innerGrooveProcessor.process (buffer);
}

juce::AudioProcessorEditor* TurntableAudioProcessor::createEditor()
{
    return new TurntableAudioProcessorEditor (*this);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TurntableAudioProcessor();
}
