#pragma once

#include <shared_plugin_helpers/shared_plugin_helpers.h>

class MidiFXProcessor : public PluginHelpers::ProcessorBase
{
public:

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        auto playHead = getPlayHead();
        auto positionInfo = playHead->getPosition();
        auto bpm = positionInfo->getBpm();
        // Initialize the MIDI sequence here, set the tempo, and create the note-on message.
        midiSequence.clear();

        auto noteOnKickMessage = juce::MidiMessage::noteOn((int)1, (int)36, (float)0.8);
        auto noteOffKickMessage = juce::MidiMessage::noteOff((int)1, (int)36);
        auto noteOnSnareMessage = juce::MidiMessage::noteOn((int)1, (int)39, (float)0.8);
        auto noteOffSnareMessage = juce::MidiMessage::noteOff((int)1, (int)39);

        for (int i = 0; i < 4; ++i)
        {
            if (i == 0 || i == 2) {
                midiSequence.addEvent(noteOnKickMessage, i);
                midiSequence.addEvent(noteOffKickMessage, i + .001);
            } else{
                // Add note-on events at quarter note intervals within a 1-bar loop.
                midiSequence.addEvent(noteOnSnareMessage, i);
                midiSequence.addEvent(noteOffSnareMessage, i + .001);
            }
        }
    }


private:

    juce::MidiBuffer tempBuffer;

    juce::MidiMessageSequence midiSequence{};

};
