// EightTrackComponent.h
// Power Station Studios — 8-Track module
//
// Replaces the earlier placeholder editor (a bare button + text label)
// with an actual cosmetic design: wood-top chassis, cartridge slot,
// twin VU meters, and a transport button row — modeled on the general
// shape of a classic console 8-track player, not a reproduction of any
// specific branded unit's logo or livery.
//
// Known gaps:
//   - VU needles are cosmetic only — driven by a random jitter while
//     "playing", not by actual signal level. Real level metering would
//     need peak/RMS measurement wired in from processBlock().
//   - Power/Auto/Fast Fwd/Pause buttons are drawn and clickable but not
//     wired to anything — only the cartridge-eject-style "Program"
//     button is functional (calls onSwitchProgram).

#pragma once
#include <JuceHeader.h>

class EightTrackComponent : public juce::Component, public juce::Timer
{
public:
    EightTrackComponent()
    {
        for (auto* b : { &powerButton, &autoButton, &ejectButton, &fastFwdButton, &pauseButton })
        {
            addAndMakeVisible (*b);
            b->setColour (juce::TextButton::buttonColourId, juce::Colour::fromRGB (70, 68, 64));
            b->setColour (juce::TextButton::textColourOffId, juce::Colours::whitesmoke);
        }

        addAndMakeVisible (programButton);
        programButton.setColour (juce::TextButton::buttonColourId, juce::Colour::fromRGB (150, 40, 36));
        programButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
        programButton.onClick = [this] { if (onSwitchProgram) onSwitchProgram(); };

        powerButton.setClickingTogglesState (true);
        powerButton.onClick = [this] { if (onPowerToggled) onPowerToggled (powerButton.getToggleState()); };

        startTimerHz (30);
    }

    std::function<void()> onSwitchProgram;
    std::function<void (bool isPowered)> onPowerToggled;

    void setCurrentProgram (int program1to4)
    {
        currentProgram = juce::jlimit (1, 4, program1to4);
        repaint();
    }

    void setPlaying (bool shouldPlay)
    {
        isPlaying = shouldPlay;
        if (! isPlaying)
        {
            leftVu = 0.0f;
            rightVu = 0.0f;
        }
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (14);

        auto buttonRow = bounds.removeFromBottom (40);
        int buttonWidth = buttonRow.getWidth() / 6;
        powerButton.setBounds (buttonRow.removeFromLeft (buttonWidth).reduced (3));
        autoButton.setBounds (buttonRow.removeFromLeft (buttonWidth).reduced (3));
        ejectButton.setBounds (buttonRow.removeFromLeft (buttonWidth).reduced (3));
        programButton.setBounds (buttonRow.removeFromLeft (buttonWidth).reduced (3));
        fastFwdButton.setBounds (buttonRow.removeFromLeft (buttonWidth).reduced (3));
        pauseButton.setBounds (buttonRow.removeFromLeft (buttonWidth).reduced (3));
    }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Wood-tone top panel
        juce::ColourGradient woodGradient (juce::Colour::fromRGB (92, 62, 40), bounds.getX(), bounds.getY(),
                                            juce::Colour::fromRGB (62, 40, 26), bounds.getX(), bounds.getBottom(), false);
        g.setGradientFill (woodGradient);
        g.fillRoundedRectangle (bounds, 8.0f);

        // Brushed-metal face plate, inset from the wood edge
        auto faceBounds = bounds.reduced (bounds.getWidth() * 0.04f, bounds.getHeight() * 0.18f);
        g.setColour (juce::Colour::fromRGB (176, 176, 180));
        g.fillRoundedRectangle (faceBounds, 4.0f);
        g.setColour (juce::Colour::fromRGB (120, 120, 124));
        g.drawRoundedRectangle (faceBounds.reduced (1.0f), 4.0f, 1.5f);

        auto innerBounds = faceBounds.reduced (faceBounds.getWidth() * 0.03f, faceBounds.getHeight() * 0.08f);

        // Cartridge slot (left third)
        auto slotArea = innerBounds.removeFromLeft (innerBounds.getWidth() * 0.32f);
        drawCartridgeSlot (g, slotArea);

        innerBounds.removeFromLeft (innerBounds.getWidth() * 0.04f);

        // VU meters (remaining area, split L/R)
        auto vuArea = innerBounds;
        auto leftMeterArea = vuArea.removeFromLeft (vuArea.getWidth() * 0.48f);
        vuArea.removeFromLeft (vuArea.getWidth() * 0.08f);
        auto rightMeterArea = vuArea;

        drawVuMeter (g, leftMeterArea, leftVu, "L");
        drawVuMeter (g, rightMeterArea, rightVu, "R");

        // Program indicator (four segments, current one lit)
        drawProgramIndicator (g, bounds);
    }

private:
    void drawCartridgeSlot (juce::Graphics& g, juce::Rectangle<float> area)
    {
        area = area.reduced (area.getWidth() * 0.08f, area.getHeight() * 0.12f);

        g.setColour (juce::Colours::black);
        g.fillRoundedRectangle (area, 3.0f);

        // Inserted cartridge, partially visible
        auto cartridge = area.reduced (area.getWidth() * 0.08f, area.getHeight() * 0.15f);
        cartridge = cartridge.withHeight (cartridge.getHeight() * 0.7f);
        g.setColour (juce::Colour::fromRGB (160, 40, 30));
        g.fillRoundedRectangle (cartridge, 2.0f);
        g.setColour (juce::Colours::black.withAlpha (0.4f));
        g.drawRoundedRectangle (cartridge, 2.0f, 1.0f);
    }

    void drawVuMeter (juce::Graphics& g, juce::Rectangle<float> area, float level, const juce::String& label)
    {
        area = area.reduced (area.getWidth() * 0.05f);

        g.setColour (juce::Colour::fromRGB (30, 24, 16));
        g.fillRoundedRectangle (area, 3.0f);

        auto center = juce::Point<float> (area.getCentreX(), area.getBottom() - area.getHeight() * 0.12f);
        float needleLength = area.getHeight() * 0.75f;

        // Arc scale
        g.setColour (juce::Colours::goldenrod.withAlpha (0.6f));
        juce::Path arc;
        arc.addArc (center.x - needleLength, center.y - needleLength, needleLength * 2.0f, needleLength * 2.0f,
                    juce::degreesToRadians (200.0f), juce::degreesToRadians (340.0f), true);
        g.strokePath (arc, juce::PathStrokeType (1.5f));

        // Needle: -40 (left, level 0) to +40 degrees (right, level 1) off vertical
        float angle = juce::jmap (level, 0.0f, 1.0f,
                                   juce::degreesToRadians (-50.0f), juce::degreesToRadians (50.0f));
        juce::Point<float> needleTip (center.x + needleLength * std::sin (angle),
                                       center.y - needleLength * std::cos (angle));
        g.setColour (juce::Colours::black);
        g.drawLine (center.x, center.y, needleTip.x, needleTip.y, 2.0f);
        g.fillEllipse (center.x - 3.0f, center.y - 3.0f, 6.0f, 6.0f);

        g.setColour (juce::Colours::whitesmoke);
        g.setFont (juce::Font (juce::jmax (10.0f, area.getHeight() * 0.12f), juce::Font::bold));
        g.drawText (label, area.removeFromTop (area.getHeight() * 0.2f), juce::Justification::centred);
    }

    void drawProgramIndicator (juce::Graphics& g, juce::Rectangle<float> fullBounds)
    {
        auto area = fullBounds.removeFromTop (fullBounds.getHeight() * 0.16f)
                              .withSizeKeepingCentre (fullBounds.getWidth() * 0.4f, fullBounds.getHeight() * 0.1f);

        float segmentWidth = area.getWidth() / 4.0f;
        for (int i = 1; i <= 4; ++i)
        {
            auto segment = juce::Rectangle<float> (area.getX() + segmentWidth * (i - 1), area.getY(),
                                                     segmentWidth - 4.0f, area.getHeight());
            g.setColour (i == currentProgram ? juce::Colours::orangered : juce::Colour::fromRGB (60, 60, 64));
            g.fillRoundedRectangle (segment, 2.0f);

            g.setColour (juce::Colours::black);
            g.setFont (juce::Font (segment.getHeight() * 0.6f, juce::Font::bold));
            g.drawText (juce::String (i), segment, juce::Justification::centred);
        }
    }

    void timerCallback() override
    {
        if (isPlaying)
        {
            leftVu  = juce::jlimit (0.0f, 1.0f, leftVu  + random.nextFloat() * 0.3f - 0.15f);
            rightVu = juce::jlimit (0.0f, 1.0f, rightVu + random.nextFloat() * 0.3f - 0.15f);
        }
        repaint();
    }

    juce::TextButton powerButton   { "Power" };
    juce::TextButton autoButton    { "Auto" };
    juce::TextButton ejectButton   { "Eject" };
    juce::TextButton programButton { "Program" };
    juce::TextButton fastFwdButton { "Fwd" };
    juce::TextButton pauseButton   { "Pause" };

    int currentProgram = 1;
    bool isPlaying = false;
    float leftVu = 0.0f;
    float rightVu = 0.0f;
    juce::Random random;
};
