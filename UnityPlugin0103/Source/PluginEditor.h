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
class UnityPlugin0103AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    UnityPlugin0103AudioProcessorEditor (UnityPlugin0103AudioProcessor&);
    ~UnityPlugin0103AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    UnityPlugin0103AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnityPlugin0103AudioProcessorEditor)
};
