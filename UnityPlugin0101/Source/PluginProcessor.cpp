/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UnityPlugin0101AudioProcessor::UnityPlugin0101AudioProcessor()
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
    vts.createAndAddParameter("gain", "Gain Control", String(), NormalisableRange<float> (0.0f, 1.0f, 0.05f), 0.0f, nullptr, nullptr);
    
    vts.state = ValueTree(Identifier(JucePlugin_Name));
}

UnityPlugin0101AudioProcessor::~UnityPlugin0101AudioProcessor()
{
}

//==============================================================================
const String UnityPlugin0101AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool UnityPlugin0101AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool UnityPlugin0101AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool UnityPlugin0101AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double UnityPlugin0101AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int UnityPlugin0101AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int UnityPlugin0101AudioProcessor::getCurrentProgram()
{
    return 0;
}

void UnityPlugin0101AudioProcessor::setCurrentProgram (int index)
{
}

const String UnityPlugin0101AudioProcessor::getProgramName (int index)
{
    return {};
}

void UnityPlugin0101AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void UnityPlugin0101AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void UnityPlugin0101AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UnityPlugin0101AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void UnityPlugin0101AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const float p = *vts.getRawParameterValue("gain");
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
bool UnityPlugin0101AudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* UnityPlugin0101AudioProcessor::createEditor()
{
    return nullptr;
}

//==============================================================================
void UnityPlugin0101AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void UnityPlugin0101AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UnityPlugin0101AudioProcessor();
}
