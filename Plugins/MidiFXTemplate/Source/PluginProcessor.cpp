#include "PluginProcessor.h"

void MidiFXProcessor::processBlock(juce::AudioBuffer<float>& audioBuffer,
                                   juce::MidiBuffer& midiMessages)

{
    tempBuffer.clear();
    audioBuffer.clear();

    for (auto m: midiMessages)
    {
        auto message = m.getMessage();
        auto samplePos = m.samplePosition;

        //First, debug all incoming messages to console:
        std::cout << "Incoming:" << message.getDescription()
                  << " SamplePos:" << juce::String(samplePos) << std::endl;

        //Then, transpose all notes to be note #60
        if (message.isNoteOnOrOff())
            message.setNoteNumber(MidiFXProcessor::getClosestCMajorScaleNote(message.getNoteNumber()));

        tempBuffer.addEvent(message, samplePos);
    }

    midiMessages.swapWith(tempBuffer);
}

juce::AudioProcessorEditor* MidiFXProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

// Function to check if a MIDI note number is in the C major scale and return the closest note
int MidiFXProcessor::getClosestCMajorScaleNote(int midiNote)
{
    // MIDI note numbers for the C major scale
    std::vector<int> cMajorScale = {0, 2, 4, 5, 7, 9, 11};  // C, D, E, F, G, A, B

    // Calculate the closest note in the C major scale
    int closestNote = midiNote;
    int minDistance = 128;  // Set an initial large value

    for (int note : cMajorScale) {
        for (int octave = 0; octave <= 10; octave++) {
            int cNote = note + octave * 12;
            int distance = std::abs(midiNote - cNote);

            if (distance < minDistance) {
                minDistance = distance;
                closestNote = cNote;
            }
        }
    }

    return closestNote;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiFXProcessor();
}
