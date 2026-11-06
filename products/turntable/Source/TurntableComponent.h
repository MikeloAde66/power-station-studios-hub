// TurntableComponent.h
// Power Station Studios — Turntable module
//
// Platter/tonearm rendering + 33/45 speed switches, sized to read clearly
// at a real plugin-window scale (previous version was a thin wrapper
// drawn inside a 420x420 editor — too small to read the groove detail or
// tonearm). Modeled on the general shape of a classic direct-drive
// turntable (metal plinth, speed buttons, tonearm with counterweight) —
// not a reproduction of any specific branded unit's logo or livery.
//
// Known gaps (unchanged from before, still real):
//   - Tonearm target groove position is a fixed resting spot, not driven
//     by playback progress (see InnerGrooveLossProcessor.h's `progress`,
//     still not wired to this component).
//   - No pitch-bend gesture handling.
//   - No album-art loading UI, no noise-preset controls.

#pragma once
#include <JuceHeader.h>

class TurntableComponent : public juce::Component, public juce::Timer
{
public:
    TurntableComponent()
    {
        addAndMakeVisible (speed33Button);
        addAndMakeVisible (speed45Button);

        speed33Button.setClickingTogglesState (true);
        speed45Button.setClickingTogglesState (true);
        speed33Button.setRadioGroupId (speedRadioGroupId);
        speed45Button.setRadioGroupId (speedRadioGroupId);
        speed33Button.setToggleState (true, juce::dontSendNotification);

        speed33Button.onClick = [this] { if (onSpeedChanged) onSpeedChanged (33.333f); };
        speed45Button.onClick = [this] { if (onSpeedChanged) onSpeedChanged (45.0f); };

        startTimerHz (60); // 60 FPS smooth platter rotation and tonearm animation
    }

    void setPlaying (bool shouldPlay, float rpm = 33.333f)
    {
        isPlaying = shouldPlay;
        currentRPM = rpm;
    }

    std::function<void (float newRpm)> onSpeedChanged;

    void timerCallback() override
    {
        if (isPlaying)
        {
            float rps = currentRPM / 60.0f;
            rotationAngle += rps * juce::MathConstants<float>::twoPi / 60.0f;
            if (rotationAngle > juce::MathConstants<float>::twoPi)
                rotationAngle -= juce::MathConstants<float>::twoPi;

            tonearmAngle = juce::jlimit (0.0f, 1.0f, tonearmAngle + 0.05f);
        }
        else
        {
            tonearmAngle = juce::jlimit (0.0f, 1.0f, tonearmAngle - 0.05f);
        }
        repaint();
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto speedRow = bounds.removeFromBottom (36).reduced (12, 4);
        speed33Button.setBounds (speedRow.removeFromLeft (speedRow.getWidth() / 2).reduced (4, 0));
        speed45Button.setBounds (speedRow.reduced (4, 0));
    }

    void paint (juce::Graphics& g) override
    {
        auto fullBounds = getLocalBounds().toFloat();
        auto platterArea = fullBounds.withTrimmedBottom (40.0f); // leave room for speed buttons
        auto center = platterArea.getCentre();
        float radius = juce::jmin (platterArea.getWidth(), platterArea.getHeight()) * 0.46f;

        // Metal plinth / chassis base
        g.setColour (juce::Colour::fromRGB (32, 32, 36));
        g.fillRoundedRectangle (fullBounds, 10.0f);
        g.setColour (juce::Colour::fromRGB (60, 60, 66));
        g.drawRoundedRectangle (fullBounds.reduced (1.5f), 10.0f, 1.5f);

        // Platter rim (brushed metal look)
        juce::ColourGradient rimGradient (juce::Colour::fromRGB (120, 120, 126), center.x - radius, center.y - radius,
                                           juce::Colour::fromRGB (70, 70, 76), center.x + radius, center.y + radius, false);
        g.setGradientFill (rimGradient);
        g.fillEllipse (center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);

        // Rotating record
        g.saveState();
        g.addTransform (juce::AffineTransform::rotation (rotationAngle, center.x, center.y));

        float recordRadius = radius * 0.94f;
        g.setColour (juce::Colours::black);
        g.fillEllipse (center.x - recordRadius, center.y - recordRadius, recordRadius * 2.0f, recordRadius * 2.0f);

        // Vinyl grooves
        g.setColour (juce::Colour::fromRGB (42, 42, 46));
        for (float r = recordRadius * 0.35f; r < recordRadius * 0.97f; r += recordRadius * 0.045f)
            g.drawEllipse (center.x - r, center.y - r, r * 2.0f, r * 2.0f, 0.6f);

        // Center label
        float labelRadius = recordRadius * 0.28f;
        g.setColour (juce::Colours::darkred);
        g.fillEllipse (center.x - labelRadius, center.y - labelRadius, labelRadius * 2.0f, labelRadius * 2.0f);

        g.setColour (juce::Colours::gold);
        g.setFont (juce::Font (labelRadius * 0.3f, juce::Font::bold));
        g.drawText ("POWER STATION", center.x - labelRadius, center.y - labelRadius * 0.22f,
                    labelRadius * 2.0f, labelRadius * 0.44f, juce::Justification::centred);

        // Spindle
        g.setColour (juce::Colours::silver);
        g.fillEllipse (center.x - 6.0f, center.y - 6.0f, 12.0f, 12.0f);
        g.restoreState();

        drawTonearm (g, center, radius);
    }

private:
    void drawTonearm (juce::Graphics& g, juce::Point<float> center, float radius)
    {
        juce::Point<float> pivot (center.x + radius * 0.92f, center.y - radius * 0.92f);

        float targetGrooveX = center.x + radius * 0.82f;
        float targetGrooveY = center.y + radius * 0.48f;
        float restX = pivot.x + 26.0f;
        float restY = pivot.y - 38.0f;

        float needleX = juce::jmap (tonearmAngle, 0.0f, 1.0f, restX, targetGrooveX);
        float needleY = juce::jmap (tonearmAngle, 0.0f, 1.0f, restY, targetGrooveY);

        // Counterweight
        g.setColour (juce::Colours::darkgrey);
        g.fillEllipse (pivot.x - 8.0f, pivot.y - 20.0f, 16.0f, 16.0f);

        // Arm
        g.setColour (juce::Colours::silver);
        g.drawLine (pivot.x, pivot.y, needleX, needleY, 5.0f);

        // Pivot base
        g.setColour (juce::Colours::lightgrey);
        g.fillEllipse (pivot.x - 7.0f, pivot.y - 7.0f, 14.0f, 14.0f);

        // Headshell / cartridge
        g.setColour (juce::Colours::darkgrey);
        g.fillEllipse (needleX - 7.0f, needleY - 7.0f, 14.0f, 14.0f);
    }

    static constexpr int speedRadioGroupId = 1001;

    juce::TextButton speed33Button { "33" };
    juce::TextButton speed45Button { "45" };

    bool isPlaying = false;
    float currentRPM = 33.333f;
    float rotationAngle = 0.0f;
    float tonearmAngle = 0.0f;
};
