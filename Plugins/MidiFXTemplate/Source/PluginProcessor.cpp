#include "PluginProcessor.h"

MidiFXProcessor::MidiFXProcessor()
    : apvts(*this, nullptr, "Parameters", createParameters())
{

}

MidiFXProcessor::~MidiFXProcessor()
{
    // Destructor code...
}

void MidiFXProcessor::processBlock(juce::AudioBuffer<float>& audioBuffer,
                                   juce::MidiBuffer& midiMessages)

{
    tempBuffer.clear();
    audioBuffer.clear();

    // Get the AudioParameterInt by ID
    auto* kickDensityParam = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("KDENSITY"));

    auto* snareDensityParam = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("SDENSITY"));

    if (kickDensityParam != nullptr && snareDensityParam != nullptr)
    {
        auto kickDensityParamValue = kickDensityParam->get();
        auto snareDensityParamValue = snareDensityParam->get();

        if(kickDensityParamValue != previousKickDensityParamValue || snareDensityParamValue != previousSnareDensityParamValue) {
            updateMidiSequence(kickDensityParamValue, snareDensityParamValue);
            previousKickDensityParamValue = kickDensityParamValue;
            previousSnareDensityParamValue = snareDensityParamValue;
            DBG("UPDATED PARAM UPDATES MIDI");
        }
//
//        if(snareDensityParamValue != previousSnareDensityParamValue) {
//            updateMidiSequence(kickDensityParamValue, snareDensityParamValue);
//            previousSnareDensityParamValue = snareDensityParamValue;
//            DBG("UPDATED SNARE");
//        }
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
        auto playHeadPosInQn = positionInfo->getPpqPosition(); // /** The current play position, in units of quarter-notes. */
        // Note all timings should be modulus of 2 (as we want to loop 2 qn)
        auto partialNoteLengthAtStartInQn = std::fmod(*playHeadPosInQn, 2.0);
        auto bufferEndPosInQn = partialNoteLengthAtStartInQn + bufferLengthPpq;

        // Calculate samples per quarter note
        auto samplesPerQuarterNote = int(60.0 / *bpm * fs);

        constructTempBuffer(
            partialNoteLengthAtStartInQn,
            bufferEndPosInQn,
            samplesPerQuarterNote);

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
