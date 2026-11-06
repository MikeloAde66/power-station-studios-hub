#include "EightTrackPluginEditor.h"

EightTrackAudioProcessorEditor::EightTrackAudioProcessorEditor (EightTrackAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible (cartridgeView);

    cartridgeView.onSwitchProgram = [this]
    {
        audioProcessor.switchProgram();
        cartridgeView.setCurrentProgram (audioProcessor.getCurrentCartridgeProgram());
    };

    cartridgeView.onPowerToggled = [this] (bool isPowered)
    {
        audioProcessor.setPowered (isPowered);
        cartridgeView.setPlaying (isPowered);
    };

    cartridgeView.setCurrentProgram (audioProcessor.getCurrentCartridgeProgram());

    setSize (520, 280);
    startTimerHz (10);
}

void EightTrackAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void EightTrackAudioProcessorEditor::resized()
{
    cartridgeView.setBounds (getLocalBounds());
}

void EightTrackAudioProcessorEditor::timerCallback()
{
    cartridgeView.setCurrentProgram (audioProcessor.getCurrentCartridgeProgram());
}
