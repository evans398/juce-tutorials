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
    juce::Slider snareDensitySlider;
    juce::Slider kickDensitySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> snareDensitySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> kickDensitySliderAttachment;

    MidiFXProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiFXEditor)
};
