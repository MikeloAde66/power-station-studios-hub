// PatchJackComponent.h
// Power Station Studios — central hub, patch bay UI
//
// A single input/output jack socket on the patch bay. Click-to-start-drag
// is wired via onJackClicked; the owning PatchBayMatrixComponent decides
// what a drag/drop actually connects.

#pragma once
#include <JuceHeader.h>

class PatchJackComponent : public juce::Component
{
public:
    PatchJackComponent (const juce::String& name, int id) : jackName (name), jackID (id) {}

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced (2.0f);

        // Outer metallic ring
        g.setColour (juce::Colours::darkgrey);
        g.fillEllipse (bounds);

        g.setColour (isPlugged ? juce::Colours::gold : juce::Colours::lightgrey);
        g.drawEllipse (bounds, 2.0f);

        // Inner hole / socket contact
        g.setColour (juce::Colours::black);
        g.fillEllipse (bounds.reduced (4.0f));
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (onJackClicked)
            onJackClicked (this, jackID, getBounds().getCentre().toFloat());
    }

    std::function<void(PatchJackComponent*, int, juce::Point<float>)> onJackClicked;
    bool isPlugged = false;

private:
    juce::String jackName;
    int jackID;
};
