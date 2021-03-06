/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class UnityPlugin0101AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    UnityPlugin0101AudioProcessorEditor (UnityPlugin0101AudioProcessor&);
    ~UnityPlugin0101AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    UnityPlugin0101AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnityPlugin0101AudioProcessorEditor)
};
