// Main.cpp
// Power Station Studios — hub entry point
//
// Minimal JUCEApplication + top-level window hosting the patch bay UI.
// This gets PowerStationHub compiling and launchable; it does NOT yet
// wire the AudioProcessorGraph in PowerStationStudioHub.h to an actual
// audio device, and the patch bay has no real nodes to connect to yet
// (no plugin instances loaded). That's the next real piece of work, not
// something papered over here.

#include <JuceHeader.h>
#include "PatchBayMatrixComponent.h"

//==============================================================================
class MainComponent : public juce::Component
{
public:
    MainComponent()
    {
        addAndMakeVisible (patchBay);
        setSize (900, 600);
    }

    void resized() override
    {
        patchBay.setBounds (getLocalBounds());
    }

private:
    PatchBayMatrixComponent patchBay;
};

//==============================================================================
class MainWindow : public juce::DocumentWindow
{
public:
    explicit MainWindow (const juce::String& name)
        : DocumentWindow (name,
                           juce::Desktop::getInstance().getDefaultLookAndFeel()
                               .findColour (juce::ResizableWindow::backgroundColourId),
                           DocumentWindow::allButtons)
    {
        setUsingNativeTitleBar (true);
        setContentOwned (new MainComponent(), true);

        centreWithSize (getWidth(), getHeight());
        setVisible (true);
        setResizable (true, true);
    }

    void closeButtonPressed() override
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
};

//==============================================================================
class PowerStationHubApplication : public juce::JUCEApplication
{
public:
    PowerStationHubApplication() = default;

    const juce::String getApplicationName() override       { return "Power Station Studios"; }
    const juce::String getApplicationVersion() override    { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override              { return true; }

    void initialise (const juce::String&) override
    {
        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
START_JUCE_APPLICATION (PowerStationHubApplication)
