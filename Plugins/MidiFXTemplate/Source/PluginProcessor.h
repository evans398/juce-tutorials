#pragma once

#include <shared_plugin_helpers/shared_plugin_helpers.h>

class MidiFXProcessor : public PluginHelpers::ProcessorBase
{
public:

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;

    static int getClosestCMajorScaleNote(int midiNote);

private:

    juce::MidiBuffer tempBuffer;


};
