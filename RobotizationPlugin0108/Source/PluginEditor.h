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
class RobotizationPlugin0108AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    RobotizationPlugin0108AudioProcessorEditor (RobotizationPlugin0108AudioProcessor&);
    ~RobotizationPlugin0108AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RobotizationPlugin0108AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RobotizationPlugin0108AudioProcessorEditor)
};
