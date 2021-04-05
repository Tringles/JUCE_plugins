/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PlugInTestAudioProcessorEditor::PlugInTestAudioProcessorEditor (PlugInTestAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    gainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    gainSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 50, 20);
    gainSlider.setRange(-60.0f, 0.0f, 0.01f);
    gainSlider.setValue(-20.0f);
    gainSlider.addListener(this);
    addAndMakeVisible(gainSlider);
    
    setSize (400, 300);
}

PlugInTestAudioProcessorEditor::~PlugInTestAudioProcessorEditor()
{
}

//==============================================================================
void PlugInTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(Colours::black);
}

void PlugInTestAudioProcessorEditor::resized()
{
    gainSlider.setBounds(getWidth() / 2 - 50, getHeight() / 2 - 75, 100, 150);
}

void PlugInTestAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if(slider == &gainSlider)
    {
        audioProcessor.mGain = gainSlider.getValue();
    }
}
