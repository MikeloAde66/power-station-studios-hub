// TurntableComponent.h
// Power Station Studios — Turntable module
//
// Draft from the initial architecture pass (pasted into the repo as-is;
// not yet reviewed/tested against the rest of the codebase). Handles
// spinning-platter rendering, center-label branding, and motorized
// tonearm drop/lift animation at 60fps.
//
// Known gaps to resolve before this is build-ready:
//   - RPM is accepted by setPlaying() but currentRPM is never driven by
//     an actual 33/45 switch in the UI yet.
//   - Tonearm target groove position is hardcoded to a single resting
//     spot, not calculated from playback progress (see
//     InnerGrooveLossProcessor.h for the progress value this should
//     eventually sync with).
//   - No pitch-bend gesture handling yet (mentioned in the blueprint,
//     not implemented here).

#pragma once
#include <JuceHeader.h>

class TurntableComponent : public juce::Component, public juce::Timer
{
public:
    TurntableComponent()
    {
        startTimerHz (60); // 60 FPS smooth platter rotation and tonearm animation
    }

    void setPlaying (bool shouldPlay, float rpm = 33.3f)
    {
        isPlaying = shouldPlay;
        currentRPM = rpm;
    }

    void timerCallback() override
    {
        if (isPlaying)
        {
            float rps = currentRPM / 60.0f;
            rotationAngle += rps * juce::MathConstants<float>::twoPi / 60.0f;
            if (rotationAngle > juce::MathConstants<float>::twoPi)
                rotationAngle -= juce::MathConstants<float>::twoPi;

            // Smooth tonearm drop onto record
            tonearmAngle = juce::jlimit (0.0f, 1.0f, tonearmAngle + 0.05f);
        }
        else
        {
            // Return tonearm to rest position
            tonearmAngle = juce::jlimit (0.0f, 1.0f, tonearmAngle - 0.05f);
        }
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        auto center = bounds.getCentre();
        float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.45f;

        // Turntable chassis base
        g.setColour (juce::Colour::fromRGB (30, 30, 35));
        g.fillRoundedRectangle (bounds, 12.0f);

        // Rotating Vinyl Record
        g.saveState();
        g.addTransform (juce::AffineTransform::rotation (rotationAngle, center.x, center.y));

        g.setColour (juce::Colours::black);
        g.fillEllipse (center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);

        // Vinyl grooves
        g.setColour (juce::Colour::fromRGB (45, 45, 48));
        for (float r = radius * 0.4f; r < radius * 0.95f; r += 8.0f)
        {
            g.drawEllipse (center.x - r, center.y - r, r * 2.0f, r * 2.0f, 0.5f);
        }

        // Center label with custom branding
        g.setColour (juce::Colours::darkred);
        g.fillEllipse (center.x - radius * 0.3f, center.y - radius * 0.3f, radius * 0.6f, radius * 0.6f);

        g.setColour (juce::Colours::gold);
        g.setFont (11.0f);
        g.drawText ("POWER STATION", center.x - radius * 0.3f, center.y - 8.0f, radius * 0.6f, 16.0f, juce::Justification::centred);

        // Spindle hole
        g.setColour (juce::Colours::silver);
        g.fillEllipse (center.x - 5.0f, center.y - 5.0f, 10.0f, 10.0f);
        g.restoreState();

        // Motorized Tonearm
        drawTonearm (g, center, radius);
    }

private:
    void drawTonearm (juce::Graphics& g, juce::Point<float> center, float radius)
    {
        juce::Point<float> pivot (center.x + radius * 0.8f, center.y - radius * 0.8f);

        float targetGrooveX = center.x + radius * 0.85f;
        float targetGrooveY = center.y + radius * 0.5f;
        float restX = pivot.x + 20.0f;
        float restY = pivot.y - 30.0f;

        float needleX = juce::jmap (tonearmAngle, 0.0f, 1.0f, restX, targetGrooveX);
        float needleY = juce::jmap (tonearmAngle, 0.0f, 1.0f, restY, targetGrooveY);

        g.setColour (juce::Colours::silver);
        g.drawLine (pivot.x, pivot.y, needleX, needleY, 6.0f);

        g.setColour (juce::Colours::darkgrey);
        g.fillEllipse (needleX - 6.0f, needleY - 6.0f, 12.0f, 12.0f);
    }

    bool isPlaying = false;
    float currentRPM = 33.3f;
    float rotationAngle = 0.0f;
    float tonearmAngle = 0.0f;
};
