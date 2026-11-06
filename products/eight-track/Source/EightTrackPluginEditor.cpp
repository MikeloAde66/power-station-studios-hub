#include "EightTrackPluginEditor.h"

EightTrackAudioProcessorEditor::EightTrackAudioProcessorEditor (EightTrackAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible (switchProgramButton);
    switchProgramButton.onClick = [this] { audioProcessor.switchProgram(); };

    addAndMakeVisible (programLabel);
    programLabel.setJustificationType (juce::Justification::centred);
    programLabel.setFont (juce::Font (24.0f, juce::Font::bold));

    setSize (300, 160);
    startTimerHz (10);
}

void EightTrackAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void EightTrackAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (12);
    switchProgramButton.setBounds (bounds.removeFromBottom (36));
    bounds.removeFromBottom (8);
    programLabel.setBounds (bounds);
}

void EightTrackAudioProcessorEditor::timerCallback()
{
    programLabel.setText ("Program " + juce::String (audioProcessor.getCurrentCartridgeProgram()) + " / 4",
                           juce::dontSendNotification);
}
