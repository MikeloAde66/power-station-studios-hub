// PowerStationStudioHub.h
// Power Station Studios — central hub
//
// Central juce::AudioProcessorGraph host. Every standalone unit (Power
// Tape, Turntable, 8-Track, Vintage Receiver, ApexStrip) runs as an
// isolated graph node; ApexMaster sits permanently at the end of the
// chain as the master bus node.
//
// Known gaps to resolve before this is build-ready:
//   - Node pointers (tapeNode, turntableNode, stripNode, masterNode) are
//     declared but never populated — no addNode() calls yet wrapping the
//     actual plugin processors from each standalone product's codebase.
//   - No input/output juce::AudioProcessorGraph::AudioGraphIOProcessor
//     nodes have been added, so the graph isn't actually connected to the
//     host's audio in/out yet.
//   - See GraphRoutingHelpers.h for the connect/disconnect/AI-preset
//     methods meant to extend this class.

#pragma once
#include <JuceHeader.h>

class PowerStationStudioHub : public juce::AudioProcessor
{
public:
    PowerStationStudioHub()
    {
        // Initialize the graph
        audioGraph = std::make_unique<juce::AudioProcessorGraph>();
    }

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        audioGraph->setPlayConfigDetails (getTotalNumInputChannels(), getTotalNumOutputChannels(), sampleRate, samplesPerBlock);
        audioGraph->prepareToPlay (sampleRate, samplesPerBlock);
    }

    void releaseResources() override
    {
        audioGraph->releaseResources();
    }

    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        audioGraph->processBlock (buffer, midiMessages);
    }

    std::unique_ptr<juce::AudioProcessorGraph>& getGraph() { return audioGraph; }

    // Node pointers for studio gear — populated once each standalone
    // product's processor is wrapped and added to the graph.
    juce::AudioProcessorGraph::Node::Ptr tapeNode;
    juce::AudioProcessorGraph::Node::Ptr turntableNode;
    juce::AudioProcessorGraph::Node::Ptr stripNode;
    juce::AudioProcessorGraph::Node::Ptr masterNode;

private:
    std::unique_ptr<juce::AudioProcessorGraph> audioGraph;
};
