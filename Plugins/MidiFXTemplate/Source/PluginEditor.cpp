#include "PluginEditor.h"

MidiFXEditor::MidiFXEditor(MidiFXProcessor& processor)
    : juce::AudioProcessorEditor(processor), processor(processor)
{
    // Constructor code
    addAndMakeVisible(snareDensitySlider);
    snareDensitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    snareDensitySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    snareDensitySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "SDENSITY", snareDensitySlider);

    addAndMakeVisible(kickDensitySlider);
    kickDensitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    kickDensitySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    kickDensitySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "KDENSITY", kickDensitySlider);

}

MidiFXEditor::~MidiFXEditor()
{
    // Destructor code
}

void MidiFXEditor::paint(juce::Graphics& g)
{
    // Paint background or custom graphics if needed
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MidiFXEditor::resized()
{
    // Set the position and size of your UI components
    // The slider is already positioned and sized in the constructor, so no need to do it here
    snareDensitySlider.setBounds(getWidth() / 2 - 100, getHeight() / 2 - 50, 200, 100);
    kickDensitySlider.setBounds(getWidth() / 4 - 100, getHeight() / 4 - 50, 200, 100);
}
