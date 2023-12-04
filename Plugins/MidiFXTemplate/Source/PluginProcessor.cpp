#include "PluginProcessor.h"

MidiFXProcessor::MidiFXProcessor()
    : apvts(*this, nullptr, "Parameters", createParameters())
{

}

MidiFXProcessor::~MidiFXProcessor()
{
    // Destructor code...
}


auto noteOnKickMessage = juce::MidiMessage::noteOn((int)1, (int)36, (float)0.8);
auto noteOffKickMessage = juce::MidiMessage::noteOn((int)1, (int)36, (float)0.0);
auto noteOnSnareMessage = juce::MidiMessage::noteOn((int)1, (int)39, (float)0.8);
auto noteOffSnareMessage = juce::MidiMessage::noteOn((int)1, (int)39, (float)0.0);

void MidiFXProcessor::processBlock(juce::AudioBuffer<float>& audioBuffer,
                                   juce::MidiBuffer& midiMessages)

{
    tempBuffer.clear();
    audioBuffer.clear();

    // Get the AudioParameterInt by ID
    auto* snareDensityParam = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("SDENSITY"));
    int snareDensityParamValue;

    auto* kickDensityParam = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("KDENSITY"));
    int kickDensityParamValue;

    if (snareDensityParam != nullptr && kickDensityParam != nullptr)
    {
        snareDensityParamValue = snareDensityParam->get();
        kickDensityParamValue = kickDensityParam->get();
    } else {
        return;
    }

    // get Playhead info && buffer size && sample rate from host
    auto playHead = getPlayHead();
    auto positionInfo = playHead->getPosition();
    auto bpm = positionInfo->getBpm();
    auto fs = getSampleRate();
    auto buffSize = audioBuffer.getNumSamples(); //size of buffer in samples

    if (positionInfo.hasValue() && bpm.hasValue() && positionInfo->getIsPlaying())
    {
        juce::MidiMessage noteOn;
        juce::MidiMessage noteOff;
        int densityParamValue;

        // figure out the end point ppq of buffer using playHeadPosInDensityNoteLengths, buffer size and sample rate
        auto bufferLengthPpq = (buffSize) * *bpm / (fs * 60.0); // size of buffer in ppq
        auto playHeadPosInQuarterNotes = positionInfo->getPpqPosition(); // /** The current play position, in units of quarter-notes. */
        auto bufferEndPosInQuarterNotes = *playHeadPosInQuarterNotes + bufferLengthPpq;

        int roundedDownQuarterNotePos = bufferEndPosInQuarterNotes;
        if (roundedDownQuarterNotePos % 2 == 0) {
            noteOn = noteOnKickMessage;
            noteOff = noteOffKickMessage;
            densityParamValue = kickDensityParamValue;
        } else {
            noteOn = noteOnSnareMessage;
            noteOff = noteOffSnareMessage;
            densityParamValue = snareDensityParamValue;
        }

        auto playHeadPosInDensityNoteLengths = *playHeadPosInQuarterNotes * densityParamValue;
        auto bufferEndPosInDensityNoteLengths = playHeadPosInDensityNoteLengths + (bufferLengthPpq * densityParamValue);
        // Note all timings should be modulus of 4 (as we want to loop the 1 bar pattern)
        auto denominator = 4.0 * densityParamValue;
        auto partialNoteLengthAtStart = std::fmod(playHeadPosInDensityNoteLengths, denominator);
        auto partialNoteLengthAtEnd = std::fmod(bufferEndPosInDensityNoteLengths, denominator);
        // Calculate samples per quarter note
        double samplesPerQuarterNote = 60.0 / *bpm * fs;
        double samplesPerDensityNote = samplesPerQuarterNote / densityParamValue;

        for (int i = 0; i < (4.0 * densityParamValue); i++)
        {
            double halfNoteLength = i + 0.5;
            double noteStartPositionInDensityNoteLengths;
            if ((partialNoteLengthAtStart <= i && i <= partialNoteLengthAtEnd))
            {
                noteStartPositionInDensityNoteLengths = i - partialNoteLengthAtStart;
                int noteStartPositionInSamples = noteStartPositionInDensityNoteLengths * samplesPerDensityNote;
                tempBuffer.addEvent(noteOn, noteStartPositionInSamples);
                DBG("ADDED MIDI NOTE");
            }
            else if ((partialNoteLengthAtStart <= halfNoteLength && halfNoteLength <= partialNoteLengthAtEnd)) {
                noteStartPositionInDensityNoteLengths = halfNoteLength - partialNoteLengthAtStart;
                int noteStartPositionInSamples = noteStartPositionInDensityNoteLengths * samplesPerDensityNote;
                tempBuffer.addEvent(noteOff, noteStartPositionInSamples);
            }
            else if (partialNoteLengthAtStart > partialNoteLengthAtEnd && i == 0)
            {
                noteStartPositionInDensityNoteLengths = denominator - partialNoteLengthAtStart;
                int noteStartPositionInSamples = noteStartPositionInDensityNoteLengths * samplesPerDensityNote;
                tempBuffer.addEvent(noteOn, noteStartPositionInSamples);
                DBG("ADDED MIDI NOTE");
            }
        }
    }
    midiMessages.swapWith(tempBuffer);
}

juce::AudioProcessorEditor* MidiFXProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiFXProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout MidiFXProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterInt>("SDENSITY", "Snare Density", 0, 10, 1));
    params.push_back(std::make_unique<juce::AudioParameterInt>("KDENSITY", "Kick Density", 0, 10, 1));
    return {params.begin(), params.end()};
}
