// PatchBayMatrixComponent.h
// Power Station Studios — central hub, patch bay UI
//
// Parent component managing active cable drag state between jacks.
//
// Known gaps to resolve before this is build-ready:
//   - mouseUp() has a comment but no actual drop-target validation or
//     graph connection registration yet — dragging a cable currently
//     doesn't call connectNodes() in GraphRoutingHelpers.h.
//   - Only one hardcoded output/input jack pair exists; this needs to
//     become a dynamic grid generated from the hub's actual node list
//     (one jack per node per channel) rather than two fixed members.
//   - outputJack.onJackClicked is wired but inputJack's click handler
//     (for starting a drag from the input side) is not.

#pragma once
#include <JuceHeader.h>
#include "PatchCableComponent.h"
#include "PatchJackComponent.h"

class PatchBayMatrixComponent : public juce::Component
{
public:
    PatchBayMatrixComponent()
    {
        // Example jack initialization
        addAndMakeVisible (outputJack);
        addAndMakeVisible (inputJack);
        addAndMakeVisible (activeCable);

        outputJack.onJackClicked = [this] (PatchJackComponent* jack, int id, juce::Point<float> pos) {
            isDraggingCable = true;
            cableStartPos = pos;
        };
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        if (isDraggingCable)
        {
            activeCable.setEndpoints (cableStartPos, e.getPosition().toFloat(), juce::Colours::crimson);
        }
    }

    void mouseUp (const juce::MouseEvent& e) override
    {
        if (isDraggingCable)
        {
            isDraggingCable = false;
            // TODO: Validate drop target over input jack, register graph
            // connection via connectNodes() (see GraphRoutingHelpers.h),
            // snap cable to input jack center.
        }
    }

    void resized() override
    {
        outputJack.setBounds (50, 50, 30, 30);
        inputJack.setBounds (300, 200, 30, 30);
        activeCable.setBounds (getLocalBounds());
    }

private:
    PatchJackComponent outputJack { "Tape Out", 1 };
    PatchJackComponent inputJack { "ApexMaster In", 2 };
    PatchCableComponent activeCable;

    bool isDraggingCable = false;
    juce::Point<float> cableStartPos;
};
