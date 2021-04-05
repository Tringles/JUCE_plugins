/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UnityPlugin0103AudioProcessor::UnityPlugin0103AudioProcessor()
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
    addParameter ( gain = new AudioParameterFloat("gain", "Gain", NormalisableRange<float> (-80.0f, 20.0f), 0.0f) );
}

UnityPlugin0103AudioProcessor::~UnityPlugin0103AudioProcessor()
{
}

//==============================================================================
const String UnityPlugin0103AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool UnityPlugin0103AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool UnityPlugin0103AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool UnityPlugin0103AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double UnityPlugin0103AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int UnityPlugin0103AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int UnityPlugin0103AudioProcessor::getCurrentProgram()
{
    return 0;
}

void UnityPlugin0103AudioProcessor::setCurrentProgram (int index)
{
}

const String UnityPlugin0103AudioProcessor::getProgramName (int index)
{
    return {};
}

void UnityPlugin0103AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void UnityPlugin0103AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void UnityPlugin0103AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UnityPlugin0103AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void UnityPlugin0103AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const float p = gain->get();
    const int len = buffer.getNumSamples();
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int i = 0; i < len; i++)
        {
            channelData[i] = channelData[i] * Decibels::decibelsToGain(p);
        }
    }
}

//==============================================================================
bool UnityPlugin0103AudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* UnityPlugin0103AudioProcessor::createEditor()
{
    return nullptr;
}

//==============================================================================
void UnityPlugin0103AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void UnityPlugin0103AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UnityPlugin0103AudioProcessor();
}
