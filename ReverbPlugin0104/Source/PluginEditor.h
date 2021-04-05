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
class ReverbPlugin0104AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ReverbPlugin0104AudioProcessorEditor (ReverbPlugin0104AudioProcessor&);
    ~ReverbPlugin0104AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ReverbPlugin0104AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbPlugin0104AudioProcessorEditor)
};
