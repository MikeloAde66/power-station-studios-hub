// GraphRoutingHelpers.h
// Power Station Studios — central hub
//
// Connection/disconnection helpers and the AI-routing-preset entry point
// for PowerStationStudioHub. In the original draft these were written as
// if they were member functions of PowerStationStudioHub (they reference
// audioGraph and the node pointers directly) — they're kept here as a
// separate file for now rather than merged into PowerStationStudioHub.h,
// since merging them wasn't verified against that class's current shape.
// To use: add these as methods on PowerStationStudioHub, or make this a
// friend/extension that takes a PowerStationStudioHub& and operates on
// its public audioGraph/node members.
//
// IMPORTANT — applyAIRoutingPreset() below is a hardcoded single-preset
// example, not the real AI routing engine. The actual AI-assisted
// routing matrix (JSON preset schema, how the AI maps active-source →
// signal chain, fallback/override behavior) is intentionally NOT
// designed yet — see docs/ai-routing-design.md. This file is a sketch of
// what the *mechanism* for applying a resolved preset could look like
// once that design exists, not the design itself.

#pragma once
#include <JuceHeader.h>

// Example: Connect Turntable Output (Channel 0/1) to ApexStrip Input (Channel 0/1)
inline void connectNodes (juce::AudioProcessorGraph& audioGraph,
                           juce::AudioProcessorGraph::Node::Ptr sourceNode, int sourceChannel,
                           juce::AudioProcessorGraph::Node::Ptr destNode, int destChannel)
{
    audioGraph.addConnection ({
        { sourceNode->nodeID, sourceChannel },
        { destNode->nodeID,   destChannel }
    });
}

// Example: Unplugging a cable
inline void disconnectNodes (juce::AudioProcessorGraph& audioGraph,
                              juce::AudioProcessorGraph::Node::Ptr sourceNode, int sourceChannel,
                              juce::AudioProcessorGraph::Node::Ptr destNode, int destChannel)
{
    audioGraph.removeConnection ({
        { sourceNode->nodeID, sourceChannel },
        { destNode->nodeID,   destChannel }
    });
}

// Sketch only — see file header. Clears ALL graph connections before
// applying a named preset, which is destructive to any manual patching
// the user has done; the real implementation will need a way to apply a
// preset without stomping on unrelated manual connections.
inline void applyAIRoutingPresetExample (juce::AudioProcessorGraph& audioGraph,
                                          juce::AudioProcessorGraph::Node::Ptr turntableNode,
                                          juce::AudioProcessorGraph::Node::Ptr tapeNode,
                                          juce::AudioProcessorGraph::Node::Ptr stripNode,
                                          juce::AudioProcessorGraph::Node::Ptr masterNode,
                                          const juce::String& presetName)
{
    // Clear existing dynamic patch connections (keep hardware inputs/master outputs)
    audioGraph.clear();

    if (presetName == "Warm Vinyl to Tape Saturation & Master")
    {
        // 1. Turntable -> Power Tape
        connectNodes (audioGraph, turntableNode, 0, tapeNode, 0);
        connectNodes (audioGraph, turntableNode, 1, tapeNode, 1);

        // 2. Power Tape -> ApexStrip
        connectNodes (audioGraph, tapeNode, 0, stripNode, 0);
        connectNodes (audioGraph, tapeNode, 1, stripNode, 1);

        // 3. ApexStrip -> ApexMaster
        connectNodes (audioGraph, stripNode, 0, masterNode, 0);
        connectNodes (audioGraph, stripNode, 1, masterNode, 1);
    }
}
