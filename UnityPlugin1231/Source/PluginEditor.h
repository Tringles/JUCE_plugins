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
class UnityPlugin1231AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    UnityPlugin1231AudioProcessorEditor (UnityPlugin1231AudioProcessor&);
    ~UnityPlugin1231AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    UnityPlugin1231AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnityPlugin1231AudioProcessorEditor)
};
