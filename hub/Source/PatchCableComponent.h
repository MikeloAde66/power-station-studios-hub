// PatchCableComponent.h
// Power Station Studios — central hub, patch bay UI
//
// Reason-style virtual patch cable: cubic Bezier curve between two jack
// points with gravitational sag proportional to distance.

#pragma once
#include <JuceHeader.h>

class PatchCableComponent : public juce::Component
{
public:
    PatchCableComponent() = default;

    void setEndpoints (juce::Point<float> start, juce::Point<float> end, juce::Colour cableColor)
    {
        startPoint = start;
        endPoint = end;
        color = cableColor;
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        if (startPoint == endPoint)
            return;

        juce::Path cablePath;
        cablePath.startNewSubPath (startPoint);

        // Calculate gravitational sag based on distance between jacks
        float distance = startPoint.getDistanceFrom (endPoint);
        float sag = juce::jmin (distance * 0.35f, 120.0f);

        juce::Point<float> controlPoint1 (startPoint.x, startPoint.y + sag);
        juce::Point<float> controlPoint2 (endPoint.x, endPoint.y + sag);

        cablePath.cubicTo (controlPoint1, controlPoint2, endPoint);

        // Draw shadow / outer casing
        g.setColour (juce::Colours::black.withAlpha (0.6f));
        g.strokePath (cablePath, juce::PathStrokeType (8.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Draw primary colored cable body
        g.setColour (color);
        g.strokePath (cablePath, juce::PathStrokeType (5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Draw metallic cable tip highlights
        g.setColour (juce::Colours::silver);
        g.fillEllipse (startPoint.x - 4.0f, startPoint.y - 4.0f, 8.0f, 8.0f);
        g.fillEllipse (endPoint.x - 4.0f, endPoint.y - 4.0f, 8.0f, 8.0f);
    }

private:
    juce::Point<float> startPoint;
    juce::Point<float> endPoint;
    juce::Colour color { juce::Colours::red };
};
