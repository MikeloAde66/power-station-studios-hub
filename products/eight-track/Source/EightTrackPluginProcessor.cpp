#include "EightTrackPluginProcessor.h"
#include "EightTrackPluginEditor.h"

EightTrackAudioProcessor::EightTrackAudioProcessor()
    : AudioProcessor (BusesProperties()
                           .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                           .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

void EightTrackAudioProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    engine.prepare (sampleRate);
}

void EightTrackAudioProcessor::releaseResources()
{
}

bool EightTrackAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo();
}

void EightTrackAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    engine.processBlock (buffer);
}

juce::AudioProcessorEditor* EightTrackAudioProcessor::createEditor()
{
    return new EightTrackAudioProcessorEditor (*this);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EightTrackAudioProcessor();
}
