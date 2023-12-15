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

    enum class NoteType
    {
        KICK,
        SNARE
    };

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        // clear midiSequence
        midiSequence.clear();
    }

    void updateMidiSequence(int kickNoteDensity, int snareNoteDensity)
    {
//        juce::MidiMessageSequence newMidiSequence{};
        midiSequence.clear();

        auto noteOn = juce::MidiMessage::noteOn((int)1, (int)36, 0.8f);
        auto noteOff = juce::MidiMessage::noteOff((int)1, (int)36);
        for (int i = 0; i < kickNoteDensity; i++) {
            double timeAdjustmentOn = double(i)/double(kickNoteDensity);
            double timeAdjustmentOff = (double(i) + 0.5)/double(kickNoteDensity);
            midiSequence.addEvent(noteOn, timeAdjustmentOn);
            midiSequence.addEvent(noteOff, timeAdjustmentOff);
        }
//            midiSequence.clear();
//            kickMidiSequence.clear();
//            kickMidiSequence = newMidiSequence;
//            newMidiSequence.addSequence(snareMidiSequence, 0.0);
//            midiSequence = newMidiSequence;

        noteOn = juce::MidiMessage::noteOn((int)1, (int)39, (float)0.8);
        noteOff = juce::MidiMessage::noteOff((int)1, (int)39);
        for (int i = 0; i < snareNoteDensity; i++) {
            double timeAdjustmentOn = double(i)/double(snareNoteDensity);
            double timeAdjustmentOff = (double(i) + 0.5)/double(snareNoteDensity);
            midiSequence.addEvent(noteOn, timeAdjustmentOn + 1.0);
//            DBG("ADDED SNARE AT " + juce::String(timeAdjustmentOn + 1.0));
            midiSequence.addEvent(noteOff, timeAdjustmentOff + 1.0);
        }
//            midiSequence.clear();
//            snareMidiSequence.clear();
//            snareMidiSequence = tempMidiSequence;
//            tempMidiSequence.addSequence(kickMidiSequence, 0.0);
//            midiSequence = tempMidiSequence;

//        DBG(midiSequence)
    }

    void constructTempBuffer(
        double partialNoteLengthAtBufferStartInQn,
        double partialNoteLengthAtBufferEndInQn,
        int samplesPerQuarterNote)
    {
        tempBuffer.clear();
        for(auto m: midiSequence)
        {
            auto message = m->message;
            if (partialNoteLengthAtBufferStartInQn <= message.getTimeStamp()
                 && message.getTimeStamp() <= partialNoteLengthAtBufferEndInQn
                 /*&& !(partialNoteLengthAtBufferStartInQn > partialNoteLengthAtBufferEndInQn
                      && int(message.getTimeStamp()) == 0)*/)
            {
                double lengthFromBufferStartToNoteTimeStampInQn;
                lengthFromBufferStartToNoteTimeStampInQn = message.getTimeStamp() - partialNoteLengthAtBufferStartInQn;
                int noteStartPositionRelativeToBufferStartInSamples =
                    lengthFromBufferStartToNoteTimeStampInQn * samplesPerQuarterNote;
                tempBuffer.addEvent(message, noteStartPositionRelativeToBufferStartInSamples);
                //            DBG("ADDED MIDI NOTE UPPER: " + juce::String(message.getTimeStamp()));
            }
            /*else if (partialNoteLengthAtBufferStartInQn > partialNoteLengthAtBufferEndInQn
                     && int(message.getTimeStamp()) == 0)
            {
                double endOfLoopInQn = 2.0;
                double remainderOfLoopFromStartOfBuffertoEndOfLoopInQn;
                remainderOfLoopFromStartOfBuffertoEndOfLoopInQn = endOfLoopInQn - partialNoteLengthAtBufferStartInQn;
                int noteStartPositionRelativeToBufferStartInSamples =
                    remainderOfLoopFromStartOfBuffertoEndOfLoopInQn * samplesPerQuarterNote;
                tempBuffer.addEvent(message, noteStartPositionRelativeToBufferStartInSamples);
            }*/
        }
    }

private:

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    juce::MidiBuffer tempBuffer;

    juce::MidiMessageSequence midiSequence{};
    juce::MidiMessageSequence kickMidiSequence{};
    juce::MidiMessageSequence snareMidiSequence{};

    int previousKickDensityParamValue = -1;
    int previousSnareDensityParamValue = -1;
};
