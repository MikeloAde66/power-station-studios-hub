// PatchJackComponent.h
// Power Station Studios — central hub, patch bay UI
//
// A single input/output jack socket on the patch bay.
//
// FIX: the original draft only fired onJackClicked from mouseDown and
// relied on the owning PatchBayMatrixComponent's mouseDrag/mouseUp
// overrides to handle the rest of the gesture. That doesn't work in
// JUCE — once a child component handles mouseDown, it captures the
// mouse for the rest of that gesture; drag/up events do NOT
// automatically bubble to the parent just because the parent also
// defines those methods. The parent's overrides were simply never being
// called. Fixed by having this component explicitly forward all three
// phases (down/drag/up) via callbacks, each carrying the full
// MouseEvent so the receiver can call e.getEventRelativeTo(owner) to
// work in its own coordinate space.

#pragma once
#include <JuceHeader.h>

class PatchJackComponent : public juce::Component
{
public:
    PatchJackComponent (const juce::String& name, int id) : jackName (name), jackID (id) {}

    const juce::String& getJackName() const { return jackName; }
    int getJackID() const { return jackID; }

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
        if (onJackDown)
            onJackDown (this, e);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        if (onJackDragged)
            onJackDragged (this, e);
    }

    void mouseUp (const juce::MouseEvent& e) override
    {
        if (onJackUp)
            onJackUp (this, e);
    }

    // Original single-shot click callback, kept for anything still using it.
    std::function<void (PatchJackComponent*, int, juce::Point<float>)> onJackClicked;

    // Full-gesture callbacks — these are the ones PatchBayGridComponent uses.
    std::function<void (PatchJackComponent*, const juce::MouseEvent&)> onJackDown;
    std::function<void (PatchJackComponent*, const juce::MouseEvent&)> onJackDragged;
    std::function<void (PatchJackComponent*, const juce::MouseEvent&)> onJackUp;

    bool isPlugged = false;

private:
    juce::String jackName;
    int jackID;
};
