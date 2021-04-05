/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

using namespace juce;

//==============================================================================
/**
*/
class PlugInTestAudioProcessorEditor  : public AudioProcessorEditor,
                                        public Slider::Listener
{
public:
    PlugInTestAudioProcessorEditor (PlugInTestAudioProcessor&);
    ~PlugInTestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged (Slider* slider) override;

private:
    Slider gainSlider;
    
    PlugInTestAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlugInTestAudioProcessorEditor)
};
