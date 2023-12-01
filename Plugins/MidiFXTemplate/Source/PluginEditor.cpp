#include "PluginEditor.h"

MidiFXEditor::MidiFXEditor(MidiFXProcessor& processor)
    : juce::AudioProcessorEditor(processor), processor(processor)
{
    // Constructor code
    addAndMakeVisible(densitySlider);
    densitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    densitySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);

    densitySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "DENSITY", densitySlider);
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
    densitySlider.setBounds(getWidth() / 2 - 100, getHeight() / 2 - 50, 200, 100);
}
