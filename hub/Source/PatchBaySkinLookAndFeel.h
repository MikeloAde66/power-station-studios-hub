// PatchBaySkinLookAndFeel.h
// Power Station Studios — hub
//
// LookAndFeel for buttons/labels in the patch bay UI: Slate-Blue faceplate
// with brass accents (or a VintageWalnut alternative theme). Pasted in
// from an external draft with one fix applied — see note below.
//
// Scope note: the hub's current patch bay window (PatchBayMatrixComponent)
// has no juce::Button or juce::Label children yet, only hand-painted jack
// circles and cables, so wiring this in changes nothing visible today. It
// becomes relevant once real controls (AI-routing toggle, preset picker,
// etc.) are added to the hub UI. Deliberately scoped to the hub only —
// Turntable and 8-Track already have their own hand-styled buttons
// (TurntableComponent.h, EightTrackComponent.h) and weren't touched here.

#pragma once
#include <JuceHeader.h>

class PatchBaySkinLookAndFeel : public juce::LookAndFeel_V4
{
public:
    enum SkinTheme { SlateBlueBrass, VintageWalnut };

    PatchBaySkinLookAndFeel (SkinTheme initialTheme = SlateBlueBrass) : currentTheme (initialTheme) {}

    void setTheme (SkinTheme newTheme) { currentTheme = newTheme; }
    SkinTheme getTheme() const { return currentTheme; }

    // --- BUTTONS & JACK CONTROLS ---
    void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                const juce::Colour& backgroundColour,
                                bool shouldDrawButtonAsHighlighted,
                                bool shouldDrawButtonAsDown) override
    {
        juce::ignoreUnused (backgroundColour, shouldDrawButtonAsHighlighted);
        auto bounds = button.getLocalBounds().toFloat().reduced (0.5f);

        if (currentTheme == SlateBlueBrass)
        {
            // Slate blue panel look with brass ring accent
            g.setColour (shouldDrawButtonAsDown ? juce::Colour (0xFF2A3442) : juce::Colour (0xFF384454));
            g.fillRoundedRectangle (bounds, 2.0f);
            g.setColour (juce::Colour (0xFFC5A059)); // Brass accent
            g.drawRoundedRectangle (bounds, 2.0f, 1.5f);
        }
        else // VintageWalnut alternative
        {
            g.setColour (shouldDrawButtonAsDown ? juce::Colour (0xFF1E1B18) : juce::Colour (0xFF2A2521));
            g.fillRoundedRectangle (bounds, 2.0f);
            g.setColour (juce::Colour (0xFFD4B26F));
            g.drawRoundedRectangle (bounds, 2.0f, 1.0f);
        }
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::ignoreUnused (shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        g.setFont (getLabelFont (button));
        g.setColour (juce::Colours::white);
        g.drawFittedText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, 1);
    }

    // --- LABELS & TEXT ---
    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        g.setColour (juce::Colours::white); // Crisp white text enforcement
        g.setFont (getLabelFont (label));
        auto inner = label.getLocalBounds().reduced (2);
        g.drawFittedText (label.getText(), inner, label.getJustificationType(),
                           juce::jmax (1, (int) ((float) inner.getHeight() / label.getFont().getHeight())));
    }

    // FIX: the real JUCE virtual this overrides is
    // `juce::Font getLabelFont (juce::Label&)` — the original draft had
    // `getLabelFont (juce::Component&) override`, which doesn't match any
    // base virtual and is a hard compile error with `override` present
    // (not just a style issue). Takes juce::Component& only where the
    // caller passes a juce::Button (drawButtonText above), which upcasts
    // fine since TextButton derives from Component.
    juce::Font getLabelFont (juce::Label&) override
    {
        return juce::Font (13.0f, juce::Font::bold);
    }

    juce::Font getLabelFont (juce::Component&)
    {
        return juce::Font (13.0f, juce::Font::bold);
    }

private:
    SkinTheme currentTheme;
};
