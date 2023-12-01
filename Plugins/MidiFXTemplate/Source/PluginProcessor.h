#pragma once

#include <shared_plugin_helpers/shared_plugin_helpers.h>

class MidiFXProcessor : public PluginHelpers::ProcessorBase
{
public:
    MidiFXProcessor();

    ~MidiFXProcessor() override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;

    juce::AudioProcessorValueTreeState apvts;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        // clear midiSequence
        midiSequence.clear();
    }




private:

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    juce::MidiBuffer tempBuffer;

    juce::MidiMessageSequence midiSequence{};

};
