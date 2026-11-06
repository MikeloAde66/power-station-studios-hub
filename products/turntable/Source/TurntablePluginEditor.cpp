#include "TurntablePluginEditor.h"

TurntableAudioProcessorEditor::TurntableAudioProcessorEditor (TurntableAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible (turntableView);

    addAndMakeVisible (playStopButton);
    playStopButton.onClick = [this]
    {
        const bool nowPlaying = ! audioProcessor.getPlaying();
        audioProcessor.setPlaying (nowPlaying);
        turntableView.setPlaying (nowPlaying);
        playStopButton.setButtonText (nowPlaying ? "Stop" : "Play");
    };

    setSize (420, 420);
    startTimerHz (30);
}

void TurntableAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void TurntableAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    playStopButton.setBounds (bounds.removeFromBottom (36).reduced (8));
    turntableView.setBounds (bounds);
}

void TurntableAudioProcessorEditor::timerCallback()
{
    // Keep the platter view's playing state in sync with the processor in
    // case playback is toggled from elsewhere (host automation, etc.) —
    // the progress value itself isn't pushed into the view yet, since
    // TurntableComponent.h's tonearm doesn't read a progress parameter.
    turntableView.setPlaying (audioProcessor.getPlaying());
}
