/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverbPlugin0104AudioProcessor::ReverbPlugin0104AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(roomSize = new AudioParameterFloat("roomSize", "Room Size", NormalisableRange<float> (0.0f, 1.0f), 1.0f));
    addParameter(damping = new AudioParameterFloat("damping", "Damping", NormalisableRange<float> (0.0f, 1.0f), 1.0f));
    addParameter(wetLevel = new AudioParameterFloat("wetLevel", "Wet Level", NormalisableRange<float> (0.0f, 1.0f), 1.0f));
    addParameter(dryLevel = new AudioParameterFloat("dryLevel", "Dry Level", NormalisableRange<float> (0.0f, 1.0f), 1.0f));
    addParameter(width = new AudioParameterFloat("width", "Width", NormalisableRange<float> (0.0f, 1.0f), 1.0f));
}

ReverbPlugin0104AudioProcessor::~ReverbPlugin0104AudioProcessor()
{
}

//==============================================================================
const String ReverbPlugin0104AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ReverbPlugin0104AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ReverbPlugin0104AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ReverbPlugin0104AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ReverbPlugin0104AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ReverbPlugin0104AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ReverbPlugin0104AudioProcessor::getCurrentProgram()
{
    return 0;
}

void ReverbPlugin0104AudioProcessor::setCurrentProgram (int index)
{
}

const String ReverbPlugin0104AudioProcessor::getProgramName (int index)
{
    return {};
}

void ReverbPlugin0104AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ReverbPlugin0104AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    reverb.setSampleRate(sampleRate);
}

void ReverbPlugin0104AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ReverbPlugin0104AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ReverbPlugin0104AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    Reverb::Parameters reverbParams;
    reverbParams.damping = *damping;
    reverbParams.dryLevel = *dryLevel;
    reverbParams.roomSize = *roomSize;
    reverbParams.wetLevel = *wetLevel;
    reverbParams.width = *width;
    reverb.setParameters(reverbParams);
    
    const auto numChannels = jmin (totalNumInputChannels, totalNumOutputChannels);
        
        if (numChannels == 1)
            reverb.processMono (buffer.getWritePointer (0), buffer.getNumSamples());

        else if (numChannels == 2)
            reverb.processStereo (buffer.getWritePointer (0), buffer.getWritePointer (1), buffer.getNumSamples());

}

//==============================================================================
bool ReverbPlugin0104AudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ReverbPlugin0104AudioProcessor::createEditor()
{
    return nullptr;
}

//==============================================================================
void ReverbPlugin0104AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ReverbPlugin0104AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverbPlugin0104AudioProcessor();
}
