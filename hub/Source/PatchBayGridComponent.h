// PatchBayGridComponent.h
// Power Station Studios — central hub, patch bay UI
//
// Replaces the earlier PatchBayMatrixComponent (two floating test jacks,
// no real layout) with an actual multi-row patch bay: labeled rows
// (MIC IN / CHNL INSERT / FX SEND-RTN / MON MIX / MULTIS), each a strip
// of labeled jacks (A1-A16, B1-B16, ...), on a metal chassis with a
// branding plate — matching the shape of the reference image, with our
// own labeling rather than copying that image's inconsistent/duplicate
// column labels verbatim.
//
// Known gaps:
//   - Connections are visual only — dragging a cable between two jacks
//     draws it and marks both jacks "plugged" (gold ring), but does NOT
//     call into GraphRoutingHelpers.h to register a real
//     AudioProcessorGraph connection. No real plugin nodes are loaded
//     into this hub yet for there to be anything to connect.
//   - No disconnect interaction yet (e.g. right-click to unplug).
//   - Dropping a cable on empty space just cancels the drag; no visual
//     "snap back" animation.
//   - The "Patch Point Normalization" diagram from the reference image
//     is not implemented — purely decorative, lower priority than the
//     functional grid.

#pragma once
#include <JuceHeader.h>
#include "PatchCableComponent.h"
#include "PatchJackComponent.h"

class PatchBayGridComponent : public juce::Component
{
public:
    PatchBayGridComponent()
    {
        buildRows();
        addAndMakeVisible (activeCable);
        activeCable.setInterceptsMouseClicks (false, false);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().toFloat().reduced (24.0f);
        bounds.removeFromTop (40.0f); // header/title/plate area

        activeCable.setBounds (getLocalBounds());

        if (rows.isEmpty())
            return;

        float rowHeight = bounds.getHeight() / (float) rows.size();

        for (int rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
        {
            auto& row = *rows[rowIndex];
            auto rowBounds = bounds.removeFromTop (rowHeight);

            row.labelBounds = rowBounds.removeFromLeft (rowBounds.getWidth() * 0.12f);
            auto jackStrip = rowBounds.reduced (6.0f);

            if (row.jacks.isEmpty())
                continue;

            float cellWidth = jackStrip.getWidth() / (float) row.jacks.size();
            float jackSize = juce::jmin (cellWidth * 0.55f, jackStrip.getHeight() * 0.45f);

            for (int i = 0; i < row.jacks.size(); ++i)
            {
                auto* jack = row.jacks[i];
                float cellCenterX = jackStrip.getX() + cellWidth * (i + 0.5f);
                float jackTop = jackStrip.getY();
                jack->setBounds (juce::Rectangle<float> (jackSize, jackSize)
                                      .withCentre ({ cellCenterX, jackTop + jackSize * 0.5f })
                                      .toNearestInt());
            }
        }
    }

    void paint (juce::Graphics& g) override
    {
        auto full = getLocalBounds().toFloat();

        // Metal chassis
        juce::ColourGradient chassisGradient (juce::Colour::fromRGB (74, 78, 84), full.getX(), full.getY(),
                                               juce::Colour::fromRGB (48, 51, 56), full.getX(), full.getBottom(), false);
        g.setGradientFill (chassisGradient);
        g.fillRoundedRectangle (full, 10.0f);
        g.setColour (juce::Colour::fromRGB (20, 20, 22));
        g.drawRoundedRectangle (full.reduced (1.5f), 10.0f, 2.0f);

        // Corner bolts
        float boltRadius = 5.0f;
        float inset = 16.0f;
        for (auto corner : { juce::Point<float> (inset, inset),
                              juce::Point<float> (full.getWidth() - inset, inset),
                              juce::Point<float> (inset, full.getHeight() - inset),
                              juce::Point<float> (full.getWidth() - inset, full.getHeight() - inset) })
        {
            g.setColour (juce::Colours::black.withAlpha (0.5f));
            g.fillEllipse (corner.x - boltRadius, corner.y - boltRadius, boltRadius * 2.0f, boltRadius * 2.0f);
            g.setColour (juce::Colour::fromRGB (160, 160, 164));
            g.fillEllipse (corner.x - boltRadius * 0.6f, corner.y - boltRadius * 0.6f, boltRadius * 1.2f, boltRadius * 1.2f);
        }

        // Title
        g.setColour (juce::Colours::whitesmoke);
        g.setFont (juce::Font (22.0f, juce::Font::bold));
        g.drawText ("Power Station Studios", full.removeFromTop (44.0f).withTrimmedLeft (28.0f),
                    juce::Justification::centredLeft);

        // Brass "PSS" plate, top right
        auto plateBounds = juce::Rectangle<float> (70.0f, 26.0f)
                                .withRightX (getWidth() - 20.0f)
                                .withY (14.0f);
        juce::ColourGradient brassGradient (juce::Colour::fromRGB (196, 162, 86), plateBounds.getX(), plateBounds.getY(),
                                             juce::Colour::fromRGB (138, 108, 48), plateBounds.getX(), plateBounds.getBottom(), false);
        g.setGradientFill (brassGradient);
        g.fillRoundedRectangle (plateBounds, 3.0f);
        g.setColour (juce::Colours::black.withAlpha (0.6f));
        g.drawRoundedRectangle (plateBounds, 3.0f, 1.0f);
        g.setColour (juce::Colour::fromRGB (40, 30, 10));
        g.setFont (juce::Font (14.0f, juce::Font::bold));
        g.drawText ("PSS", plateBounds, juce::Justification::centred);

        // Row labels + per-jack column labels
        for (auto* row : rows)
        {
            g.setColour (juce::Colours::whitesmoke);
            g.setFont (juce::Font (13.0f, juce::Font::bold));
            g.drawFittedText (row->label, row->labelBounds.toNearestInt(), juce::Justification::centredLeft, 2);

            g.setFont (juce::Font (9.0f));
            g.setColour (juce::Colours::lightgrey);
            for (auto* jack : row->jacks)
            {
                auto labelArea = jack->getBounds().toFloat().translated (0.0f, jack->getHeight() + 2.0f)
                                      .withHeight (12.0f);
                g.drawText (jack->getJackName(), labelArea, juce::Justification::centred);
            }
        }
    }

private:
    struct JackRow
    {
        juce::String label;
        juce::OwnedArray<PatchJackComponent> jacks;
        juce::Rectangle<float> labelBounds;
    };

    void addRow (const juce::String& label, const juce::String& columnPrefix, int count)
    {
        auto* row = rows.add (new JackRow());
        row->label = label;

        for (int i = 1; i <= count; ++i)
        {
            auto* jack = row->jacks.add (new PatchJackComponent (columnPrefix + juce::String (i), nextJackID++));
            addAndMakeVisible (jack);
            wireJackCallbacks (*jack);
        }
    }

    void buildRows()
    {
        addRow ("MIC IN",       "A", 16);
        addRow ("CHNL INSERT",  "B", 16);
        addRow ("FX SEND/RTN",  "C", 16);
        addRow ("MON MIX",      "D", 8);
        addRow ("MULTIS",       "E", 8);
    }

    void wireJackCallbacks (PatchJackComponent& jack)
    {
        jack.onJackDown = [this] (PatchJackComponent* source, const juce::MouseEvent& e)
        {
            dragSourceJack = source;
            auto relative = e.getEventRelativeTo (this);
            dragStartPos = source->getBounds().getCentre().toFloat();
            activeCable.setEndpoints (dragStartPos, relative.position, juce::Colours::crimson);
        };

        jack.onJackDragged = [this] (PatchJackComponent*, const juce::MouseEvent& e)
        {
            if (dragSourceJack == nullptr)
                return;
            auto relative = e.getEventRelativeTo (this);
            activeCable.setEndpoints (dragStartPos, relative.position, juce::Colours::crimson);
        };

        jack.onJackUp = [this] (PatchJackComponent*, const juce::MouseEvent& e)
        {
            if (dragSourceJack == nullptr)
                return;

            auto relative = e.getEventRelativeTo (this);
            auto* targetJack = findJackAt (relative.position);

            if (targetJack != nullptr && targetJack != dragSourceJack)
            {
                dragSourceJack->isPlugged = true;
                targetJack->isPlugged = true;

                auto* cable = persistentCables.add (new PatchCableComponent());
                addAndMakeVisible (cable);
                cable->setBounds (getLocalBounds());
                cable->setEndpoints (dragSourceJack->getBounds().getCentre().toFloat(),
                                      targetJack->getBounds().getCentre().toFloat(),
                                      juce::Colours::crimson);
                cable->toBack(); // keep behind jacks/active cable

                dragSourceJack->repaint();
                targetJack->repaint();
            }

            dragSourceJack = nullptr;
            activeCable.setEndpoints ({}, {}, juce::Colours::transparentBlack);
        };
    }

    PatchJackComponent* findJackAt (juce::Point<float> pos)
    {
        for (auto* row : rows)
            for (auto* jack : row->jacks)
                if (jack->getBounds().toFloat().contains (pos))
                    return jack;
        return nullptr;
    }

    juce::OwnedArray<JackRow> rows;
    juce::OwnedArray<PatchCableComponent> persistentCables;
    PatchCableComponent activeCable;

    PatchJackComponent* dragSourceJack = nullptr;
    juce::Point<float> dragStartPos;
    int nextJackID = 1;
};
