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
auto noteOffKickMessage = juce::MidiMessage::noteOff((int)1, (int)36);
auto noteOnSnareMessage = juce::MidiMessage::noteOn((int)1, (int)39, (float)0.8);
auto noteOffSnareMessage = juce::MidiMessage::noteOff((int)1, (int)39);

void MidiFXProcessor::processBlock(juce::AudioBuffer<float>& audioBuffer,
                                   juce::MidiBuffer& midiMessages)

{
    tempBuffer.clear();
    audioBuffer.clear();

    // Get the AudioParameterInt by ID
    auto* densityParam = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("DENSITY"));

    if (densityParam != nullptr)
    {
        // Use myIntParameter as needed...
        int densityParamValue = densityParam->get();
    }

    // get Playhead info && buffer size && sample rate from host
    auto playHead = getPlayHead();
    auto positionInfo = playHead->getPosition();
    auto bpm = positionInfo->getBpm();
    auto fs = getSampleRate();
    auto buffSize = audioBuffer.getNumSamples();

    if (positionInfo.hasValue() && bpm.hasValue() && positionInfo->getIsPlaying())
    {
        // figure out the end point ppq of buffer using pos, buffer size and sample rate
        auto bufferLengthPpq = (buffSize) * *bpm / (fs * 60.0);
        auto pos = positionInfo->getPpqPosition();
        auto endPos = *pos + bufferLengthPpq;
        // Note all timings should be modulus of 4 (as we want to loop the 1 bar pattern)
        auto denominator = 4.0;
        auto modPosSeq = std::fmod(*pos, denominator);
        auto modEndPosSeq = std::fmod(endPos, denominator);
//        DBG("Value of PosSeq: " << *pos);
//        DBG("Value of EndPosSeq: " << endPos);
        DBG("Value of modPosSeq: " << modPosSeq);
        DBG("Value of modEndPosSeq: " << modEndPosSeq);
        // Calculate samples per quarter note
        double samplesPerQuarterNote = 60.0 / *bpm * fs;

        for (int i = 0; i < 4; i++)
        {
            double ppqPosition;
            if ((modPosSeq <= i && i <= modEndPosSeq))
            {
                ppqPosition = i - modPosSeq;
                int samplePosition = ppqPosition * samplesPerQuarterNote;
                noteOnKickMessage.setTimeStamp(0);
                tempBuffer.addEvent(noteOnKickMessage, samplePosition);
                DBG("ADDED MIDI NOTE");
            }
            else if (modPosSeq > modEndPosSeq && i == 0)
            {
                ppqPosition = denominator - modPosSeq;
                int samplePosition = ppqPosition * samplesPerQuarterNote;
                noteOnKickMessage.setTimeStamp(0);
                tempBuffer.addEvent(noteOnKickMessage, samplePosition);
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
    params.push_back(std::make_unique<juce::AudioParameterInt>("DENSITY", "Density", 0, 10, 1));
    return {params.begin(), params.end()};
}
