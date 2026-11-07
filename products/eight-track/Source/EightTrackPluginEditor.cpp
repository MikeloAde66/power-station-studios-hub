#include "EightTrackPluginEditor.h"

EightTrackAudioProcessorEditor::EightTrackAudioProcessorEditor (EightTrackAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&eightTrackSkin);

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

    // Direct 1-4 selection from the program row, distinct from the
    // cycling onSwitchProgram above -- both call into the same real
    // EightTrackCartridgeEngine, just via setCartridgeProgram() instead
    // of switchProgram().
    cartridgeView.onProgramSelected = [this] (int program1to4)
    {
        audioProcessor.setCartridgeProgram (program1to4);
        cartridgeView.setCurrentProgram (audioProcessor.getCurrentCartridgeProgram());
    };

    // No settings panel built yet -- these are stubs, an obvious place
    // to hang one later.
    cartridgeView.onOptionsClicked = [] {};
    cartridgeView.onSettingsClicked = [] {};

    cartridgeView.setCurrentProgram (audioProcessor.getCurrentCartridgeProgram());

    setSize (640, 420);
    startTimerHz (10);
}

EightTrackAudioProcessorEditor::~EightTrackAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
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
