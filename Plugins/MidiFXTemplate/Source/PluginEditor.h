#pragma once
#include "PluginProcessor.h"

class MidiFXEditor : public juce::AudioProcessorEditor
{
public:
    MidiFXEditor(MidiFXProcessor& processor);
    ~MidiFXEditor() override;

    //===================================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider densitySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> densitySliderAttachment;

    MidiFXProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiFXEditor)
};
