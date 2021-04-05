/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginParameter.h"

//==============================================================================
WahWahPlugin0104AudioProcessor::WahWahPlugin0104AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
parameters (*this)
, paramMode (parameters, "Mode", modeItemsUI, modeManual)
, paramMix (parameters, "Mix", "", 0.0f, 1.0f, 0.5f)
, paramFrequency (parameters, "Frequency", "Hz", 200.0f, 1300.0f, 300.0f,
                  [this](float value){ paramFrequency.setCurrentAndTargetValue (value); updateFilters(); return value; })
, paramQfactor (parameters, "Q Factor", "", 0.1f, 20.0f, 10.0f,
                [this](float value){ paramQfactor.setCurrentAndTargetValue (value); updateFilters(); return value; })
, paramGain (parameters, "Gain", "dB", 0.0f, 20.0f, 20.0f,
             [this](float value){ paramGain.setCurrentAndTargetValue (value); updateFilters(); return value; })
, paramFilterType (parameters, "Filter type", filterTypeItemsUI, filterTypeResonantLowPass,
                   [this](float value){ paramFilterType.setCurrentAndTargetValue (value); updateFilters(); return value; })
, paramLFOfrequency (parameters, "LFO Frequency", "Hz", 0.0f, 5.0f, 2.0f)
, paramMixLFOandEnvelope (parameters, "LFO/Env", "", 0.0f, 1.0f, 0.8f)
, paramEnvelopeAttack (parameters, "Env. Attack", "ms", 0.1f, 100.0f, 2.0f, [](float value){ return value * 0.001f; })
, paramEnvelopeRelease (parameters, "Env. Release", "ms", 10.0f, 1000.0f, 300.0f, [](float value){ return value * 0.001f; })
{
centreFrequency = paramFrequency.getTargetValue();
parameters.apvts.state = ValueTree (Identifier (getName().removeCharacters ("- ")));
}

WahWahPlugin0104AudioProcessor::~WahWahPlugin0104AudioProcessor()
{
}

//==============================================================================
void WahWahPlugin0104AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const double smoothTime = 1e-3;
    paramMode.reset (sampleRate, smoothTime);
    paramMix.reset (sampleRate, smoothTime);
    paramFrequency.reset (sampleRate, smoothTime);
    paramQfactor.reset (sampleRate, smoothTime);
    paramGain.reset (sampleRate, smoothTime);
    paramFilterType.reset (sampleRate, smoothTime);
    paramLFOfrequency.reset (sampleRate, smoothTime);
    paramMixLFOandEnvelope.reset (sampleRate, smoothTime);
    paramEnvelopeAttack.reset (sampleRate, smoothTime);
    paramEnvelopeRelease.reset (sampleRate, smoothTime);

    //======================================

    filters.clear();
    for (int i = 0; i < getTotalNumInputChannels(); ++i) {
        Filter* filter;
        filters.add (filter = new Filter());
    }
    updateFilters();

    lfoPhase = 0.0f;
    inverseSampleRate = 1.0f / (float)sampleRate;
    twoPi = 2.0f * M_PI;
    for (int i = 0; i < getTotalNumInputChannels(); ++i)
        envelopes.add (0.0f);
    inverseE = 1.0f / M_E;
}

void WahWahPlugin0104AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    const int numInputChannels = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    //======================================

    float phase;

    for (int channel = 0; channel < numInputChannels; ++channel) {
        float* channelData = buffer.getWritePointer (channel);
        phase = lfoPhase;

        for (int sample = 0; sample < numSamples; ++sample) {
            float in = channelData[sample];

            float absIn = fabs (in);
            float envelope;
            float attack = calculateAttackOrRelease (paramEnvelopeAttack.getNextValue());
            float release = calculateAttackOrRelease (paramEnvelopeRelease.getNextValue());

            if (absIn > envelopes[channel])
                envelope = attack * envelopes[channel] + (1.0f - attack) * absIn;
            else
                envelope = release * envelopes[channel] + (1.0f - release) * absIn;

            envelopes.set (channel, envelope);

            if (paramMode.getTargetValue() == modeAutomatic) {
                float centreFrequencyLFO = 0.5f + 0.5f * sinf (twoPi * phase);
                float centreFrequencyEnv = envelopes[channel];
                centreFrequency =
                    centreFrequencyLFO + paramMixLFOandEnvelope.getNextValue() * (centreFrequencyEnv - centreFrequencyLFO);

                centreFrequency *= paramFrequency.maxValue - paramFrequency.minValue;
                centreFrequency += paramFrequency.minValue;

                phase += paramLFOfrequency.getNextValue() * inverseSampleRate;
                if (phase >= 1.0f)
                    phase -= 1.0f;

                paramFrequency.setCurrentAndTargetValue (centreFrequency);
                updateFilters();
            }

            float filtered = filters[channel]->processSingleSampleRaw (in);
            float out = in + paramMix.getNextValue() * (filtered - in);
            channelData[sample] = out;
        }
    }

    lfoPhase = phase;

    for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
        buffer.clear (channel, 0, numSamples);
}

//==============================================================================
void WahWahPlugin0104AudioProcessor::updateFilters()
{
    double discreteFrequency = 2.0 * M_PI * (double)paramFrequency.getTargetValue() / getSampleRate();
    double qFactor = (double)paramQfactor.getTargetValue();
    double gain = pow (10.0, (double)paramGain.getTargetValue() * 0.05);
    int type = (int)paramFilterType.getTargetValue();

    for (int i = 0; i < filters.size(); ++i)
        filters[i]->updateCoefficients (discreteFrequency, qFactor, gain, type);
}

float WahWahPlugin0104AudioProcessor::calculateAttackOrRelease (float value)
{
    if (value == 0.0f)
        return 0.0f;
    else
        return pow (inverseE, inverseSampleRate / value);
}
//==============================================================================
void WahWahPlugin0104AudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WahWahPlugin0104AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
  ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

bool WahWahPlugin0104AudioProcessor::hasEditor() const
{
    return false;
}

juce::AudioProcessorEditor* WahWahPlugin0104AudioProcessor::createEditor()
{
    return nullptr;
}

//==============================================================================
void WahWahPlugin0104AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void WahWahPlugin0104AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

double WahWahPlugin0104AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WahWahPlugin0104AudioProcessor::getNumPrograms()
{
    return 1;
}

int WahWahPlugin0104AudioProcessor::getCurrentProgram()
{
    return 0;
}

void WahWahPlugin0104AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WahWahPlugin0104AudioProcessor::getProgramName (int index)
{
    return {};
}

void WahWahPlugin0104AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

const juce::String WahWahPlugin0104AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WahWahPlugin0104AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WahWahPlugin0104AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WahWahPlugin0104AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WahWahPlugin0104AudioProcessor();
}
