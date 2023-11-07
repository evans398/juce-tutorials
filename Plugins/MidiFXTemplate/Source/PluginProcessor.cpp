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
        
        tempBuffer.addEvent(message, samplePos);
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
