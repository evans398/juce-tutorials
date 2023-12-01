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

//    void prepareToPlay(double sampleRate, int samplesPerBlock) override
//    {
//        // Initialize the MIDI sequence here, set the tempo, and create the note-on message.
//        midiSequence.clear();
//
//        auto noteOnKickMessage = juce::MidiMessage::noteOn((int)1, (int)36, (float)0.8);
//        auto noteOffKickMessage = juce::MidiMessage::noteOff((int)1, (int)36);
//        auto noteOnSnareMessage = juce::MidiMessage::noteOn((int)1, (int)39, (float)0.8);
//        auto noteOffSnareMessage = juce::MidiMessage::noteOff((int)1, (int)39);
//
//        for (int i = 0; i < 4; ++i)
//        {
//            if (i == 0 || i == 2) {
//                midiSequence.addEvent(noteOnKickMessage, i);
//                midiSequence.addEvent(noteOffKickMessage, i + .05);
//            } else{
//                // Add note-on events at quarter note intervals within a 1-bar loop.
//                midiSequence.addEvent(noteOnSnareMessage, i);
//                midiSequence.addEvent(noteOffSnareMessage, i + .05);
//            }
//        }
//    }




private:

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    juce::MidiBuffer tempBuffer;

    juce::MidiMessageSequence midiSequence{};

};
