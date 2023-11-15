#include "PluginProcessor.h"

void MidiFXProcessor::processBlock(juce::AudioBuffer<float>& audioBuffer,
                                   juce::MidiBuffer& midiMessages)

{
    tempBuffer.clear();
    audioBuffer.clear();

    // get Playhead info && buffer size && sample rate from host
    auto playHead = getPlayHead();
    auto positionInfo = playHead->getPosition();
    auto bpm = positionInfo->getBpm();
    auto fs = getSampleRate();
    auto buffSize = audioBuffer.getNumSamples();

    if (positionInfo.hasValue() && bpm.hasValue() && positionInfo->getIsPlaying()) {
        // figure out the end point ppq of buffer using pos, buffer size and sample rate
        auto bufferLengthPpq = (buffSize) * *bpm / (fs * 60.0);
        auto pos = positionInfo->getPpqPosition();
        auto endPos = *pos + bufferLengthPpq;
        // Note all timings should be modulus of 4 (as we want to loop the 1 bar pattern)
        auto denominator = 4.0;
        auto modPosSeq = std::fmod(*pos, denominator);
        auto modEndPosSeq = std::fmod(endPos, denominator);
        DBG("Value of PosSeq: " << *pos);
        DBG("Value of EndPosSeq: " << endPos);
        // Calculate samples per quarter note
        double samplesPerQuarterNote = 60.0 / *bpm * fs;
//
        for(auto m: midiSequence) {
            auto message = m->message;
            double ppqPosition;
            if ((modPosSeq <= message.getTimeStamp() && message.getTimeStamp() <= modEndPosSeq)) {
                ppqPosition = message.getTimeStamp() - modPosSeq;
                int samplePosition = ppqPosition * samplesPerQuarterNote;
                tempBuffer.addEvent(message, samplePosition);
                DBG("ADDED MIDI NOTE");
            } else if (modPosSeq > modEndPosSeq && message.getTimeStamp() == 0) {
                ppqPosition = denominator - modPosSeq;
                int samplePosition = ppqPosition * samplesPerQuarterNote;
                tempBuffer.addEvent(message, samplePosition);
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
