/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UnityPlugin1231AudioProcessor::UnityPlugin1231AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     :  AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
        vts(*this, nullptr)
#endif
{
    vts.createAndAddParameter("gain", "Gain", String(), NormalisableRange<float> (-80.0f, 0.0f, 0.05f), 0.0f, nullptr, nullptr);
    
    vts.state = ValueTree ( Identifier(JucePlugin_Name));
}

UnityPlugin1231AudioProcessor::~UnityPlugin1231AudioProcessor()
{
}

//==============================================================================
const String UnityPlugin1231AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool UnityPlugin1231AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool UnityPlugin1231AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool UnityPlugin1231AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double UnityPlugin1231AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int UnityPlugin1231AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int UnityPlugin1231AudioProcessor::getCurrentProgram()
{
    return 0;
}

void UnityPlugin1231AudioProcessor::setCurrentProgram (int index)
{
}

const String UnityPlugin1231AudioProcessor::getProgramName (int index)
{
    return {};
}

void UnityPlugin1231AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void UnityPlugin1231AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void UnityPlugin1231AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UnityPlugin1231AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void UnityPlugin1231AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float g = *vts.getRawParameterValue("gain");
    int len = buffer.getNumSamples();
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        for (int i = 0; i < len; i++)
        {
            channelData[i] = channelData[i] * Decibels::decibelsToGain(g);
        }
    }
}

//==============================================================================
bool UnityPlugin1231AudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* UnityPlugin1231AudioProcessor::createEditor()
{
    return new UnityPlugin1231AudioProcessorEditor (*this);
}

//==============================================================================
void UnityPlugin1231AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void UnityPlugin1231AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UnityPlugin1231AudioProcessor();
}
