// EightTrackSkinLookAndFeel.h
// Power Station Studios — 8-Track module
//
// LookAndFeel for the transport button row (Power/Auto/Eject/Program/
// Fwd/Pause) in EightTrackComponent.h. The chassis, VU meters, cartridge
// slot and program-segment indicator are drawn directly in
// EightTrackComponent::paint() and are NOT affected by this class —
// LookAndFeel only touches juce::Button/juce::Label rendering.
//
// Originally pasted from Gemini as a LookAndFeel_V4 subclass. Fixed one
// bug before integrating: the draft declared
//   juce::Font getLabelFont (juce::Component&) override
// which does not match any real JUCE virtual (the actual signature is
// getLabelFont (juce::Label&)) — with `override` present that's a hard
// compile error, the exact same mistake as the earlier
// PatchBaySkinLookAndFeel paste. Fixed the same way: a correctly-signed
// override for Label, plus a second non-virtual Component& overload so
// the drawButtonText() call site (which passes a TextButton) still
// resolves without ambiguity.
//
// Known gap: two themes are implemented (VintageWalnut, IndustrialSteel)
// but nothing in the UI switches between them yet — defaults to
// VintageWalnut. setTheme()/getTheme() are ready for whenever a
// skin-select control gets added (e.g. the "Settings..." button in the
// reference mockup).

#pragma once
#include <JuceHeader.h>

class EightTrackSkinLookAndFeel : public juce::LookAndFeel_V4
{
public:
    enum SkinTheme { VintageWalnut, IndustrialSteel };

    explicit EightTrackSkinLookAndFeel (SkinTheme initialTheme = VintageWalnut)
        : currentTheme (initialTheme)
    {
        applyThemeDefaultColour();
    }

    void setTheme (SkinTheme newTheme)
    {
        currentTheme = newTheme;
        applyThemeDefaultColour();
    }

    SkinTheme getTheme() const { return currentTheme; }

    // --- BUTTONS ---
    // Originally this hardcoded one fixed colour pair per theme for every
    // button, which meant a caller could never make one button look
    // different from another (e.g. a highlighted "active program" button
    // among otherwise identical ones) — the per-button backgroundColour
    // JUCE already computes from button.findColour(buttonColourId) was
    // being thrown away. Fixed to actually use it: callers that want the
    // default theme look just don't set a custom colour (TextButton's
    // buttonColourId falls back to this LookAndFeel's colour scheme,
    // seeded per-theme in the constructor below), and callers that want a
    // specific button to stand out call button.setColour(buttonColourId,
    // ...) and this now honors it.
    void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                const juce::Colour& backgroundColour,
                                bool shouldDrawButtonAsHighlighted,
                                bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced (0.5f);

        auto fill = backgroundColour;
        if (shouldDrawButtonAsDown)
            fill = fill.darker (0.3f);
        else if (shouldDrawButtonAsHighlighted)
            fill = fill.brighter (0.1f);

        g.setColour (fill);
        g.fillRoundedRectangle (bounds, 3.0f);
        g.setColour (fill.darker (0.7f));
        g.drawRoundedRectangle (bounds, 3.0f, 1.0f);
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                          bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/) override
    {
        g.setFont (getLabelFont (button));
        auto fill = button.findColour (juce::TextButton::buttonColourId);
        g.setColour (fill.getPerceivedBrightness() > 0.55f ? juce::Colours::black : juce::Colours::white);
        g.drawFittedText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, 1);
    }

    // --- LABELS & TEXT ---
    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        g.setColour (juce::Colours::white);
        g.setFont (getLabelFont (label));
        auto inner = label.getLocalBounds().reduced (2);
        g.drawFittedText (label.getText(), inner, label.getJustificationType(),
                           juce::jmax (1, (int) ((float) inner.getHeight() / label.getFont().getHeight())));
    }

    // Real JUCE virtual — correctly signed, so `override` is valid here.
    juce::Font getLabelFont (juce::Label&) override
    {
        return juce::Font (13.0f, juce::Font::bold);
    }

    // Non-virtual overload so drawButtonText() above can call
    // getLabelFont(button) with a TextButton (a Component, not a Label)
    // without colliding with the real virtual.
    juce::Font getLabelFont (juce::Component&)
    {
        return juce::Font (13.0f, juce::Font::bold);
    }

private:
    // Sets this LookAndFeel's default TextButton colour so any button
    // that hasn't been given an explicit per-button colour (via
    // button.setColour(buttonColourId, ...)) still renders in the
    // current theme's tone. Buttons that DO set their own colour (e.g.
    // an "active program" highlight) override this per-instance.
    void applyThemeDefaultColour()
    {
        setColour (juce::TextButton::buttonColourId,
                   currentTheme == VintageWalnut ? juce::Colour (0xFFE5C158)
                                                  : juce::Colour (0xFF2D323D));
    }

    SkinTheme currentTheme;
};
