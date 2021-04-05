/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginParameter.h"
#include "PluginEditor.h"

//==============================================================================
RobotizationPlugin0108AudioProcessor::RobotizationPlugin0108AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    stft (*this), parameters (*this)
    , paramEffect (parameters, "Effect", effectItemsUI, effectPassThrough)
    , paramFftSize (parameters, "FFT size", fftSizeItemsUI, fftSize512,
                    [this](float value){
                        const ScopedLock sl (lock);
                        value = (float)(1 << ((int)value + 5));
                        paramFftSize.setCurrentAndTargetValue (value);
                        stft.updateParameters((int)paramFftSize.getTargetValue(),
                                              (int)paramHopSize.getTargetValue(),
                                              (int)paramWindowType.getTargetValue());
                        return value;
                    })
    , paramHopSize (parameters, "Hop size", hopSizeItemsUI, hopSize8,
                    [this](float value){
                        const ScopedLock sl (lock);
                        value = (float)(1 << ((int)value + 1));
                        paramHopSize.setCurrentAndTargetValue (value);
                        stft.updateParameters((int)paramFftSize.getTargetValue(),
                                              (int)paramHopSize.getTargetValue(),
                                              (int)paramWindowType.getTargetValue());
                        return value;
                    })
    , paramWindowType (parameters, "Window type", windowTypeItemsUI, STFT::windowTypeHann,
                       [this](float value){
                           const ScopedLock sl (lock);
                           paramWindowType.setCurrentAndTargetValue (value);
                           stft.updateParameters((int)paramFftSize.getTargetValue(),
                                                 (int)paramHopSize.getTargetValue(),
                                                 (int)paramWindowType.getTargetValue());
                           return value;
                       })
    {
        parameters.apvts.state = ValueTree (Identifier (getName().removeCharacters ("- ")));
    }

RobotizationPlugin0108AudioProcessor::~RobotizationPlugin0108AudioProcessor()
{
}


//==============================================================================
void RobotizationPlugin0108AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const double smoothTime = 1e-3;
    paramEffect.reset (sampleRate, smoothTime);
    paramFftSize.reset (sampleRate, smoothTime);
    paramHopSize.reset (sampleRate, smoothTime);
    paramWindowType.reset (sampleRate, smoothTime);

    //======================================

    stft.setup (getTotalNumInputChannels());
    stft.updateParameters((int)paramFftSize.getTargetValue(),
                          (int)paramHopSize.getTargetValue(),
                          (int)paramWindowType.getTargetValue());
}


void RobotizationPlugin0108AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    const ScopedLock sl (lock);

    ScopedNoDenormals noDenormals;

    const int numInputChannels = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    //======================================

    stft.processBlock (buffer);

    //======================================

    for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
        buffer.clear (channel, 0, numSamples);
}

//==============================================================================
bool RobotizationPlugin0108AudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* RobotizationPlugin0108AudioProcessor::createEditor()
{
    return nullptr;
}

//==============================================================================
void RobotizationPlugin0108AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RobotizationPlugin0108AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void RobotizationPlugin0108AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}
//==============================================================================
const String RobotizationPlugin0108AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RobotizationPlugin0108AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RobotizationPlugin0108AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RobotizationPlugin0108AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RobotizationPlugin0108AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RobotizationPlugin0108AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RobotizationPlugin0108AudioProcessor::getCurrentProgram()
{
    return 0;
}

void RobotizationPlugin0108AudioProcessor::setCurrentProgram (int index)
{
}

const String RobotizationPlugin0108AudioProcessor::getProgramName (int index)
{
    return {};
}

void RobotizationPlugin0108AudioProcessor::changeProgramName (int index, const String& newName)
{
}


#ifndef JucePlugin_PreferredChannelConfigurations
bool RobotizationPlugin0108AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

//==============================================================================


// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RobotizationPlugin0108AudioProcessor();
}
