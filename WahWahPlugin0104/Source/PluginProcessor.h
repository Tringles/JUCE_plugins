/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <JuceHeader.h>
#include "PluginParameter.h"

//==============================================================================

class WahWahPlugin0104AudioProcessor : public AudioProcessor
{
public:
    //==============================================================================

    WahWahPlugin0104AudioProcessor();
    ~WahWahPlugin0104AudioProcessor();

    //==============================================================================

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================






    //==============================================================================

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    //==============================================================================

    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds() const override;

    //==============================================================================

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================






    //==============================================================================

    StringArray modeItemsUI = {
        "Manual",
        "Automatic"
    };

    enum modeIndex {
        modeManual = 0,
        modeAutomatic,
    };

    StringArray filterTypeItemsUI = {
        "Resonant Low-pass",
        "Band-pass",
        "Peaking/Notch"
    };

    enum filterTypeIndex {
        filterTypeResonantLowPass = 0,
        filterTypeBandPass,
        filterTypePeakingNotch,
    };

    //======================================

    class Filter : public IIRFilter
    {
    public:
        void updateCoefficients (const double discreteFrequency,
                                 const double qFactor,
                                 const double gain,
                                 const int filterType) noexcept
        {
            jassert (discreteFrequency > 0);
            jassert (qFactor > 0);

            double bandwidth = jmin (discreteFrequency / qFactor, M_PI * 0.99);
            double two_cos_wc = -2.0 * cos (discreteFrequency);
            double tan_half_bw = tan (bandwidth / 2.0);
            double tan_half_wc = tan (discreteFrequency / 2.0);
            double tan_half_wc_2 = tan_half_wc * tan_half_wc;
            double sqrt_gain = sqrt (gain);

            switch (filterType) {
                case filterTypeResonantLowPass: {
                    coefficients = IIRCoefficients (/* b0 */ tan_half_wc_2,
                                                    /* b1 */ tan_half_wc_2 * 2,
                                                    /* b2 */ tan_half_wc_2,
                                                    /* a0 */ tan_half_wc_2 + tan_half_wc / gain + 1.0,
                                                    /* a1 */ 2 * tan_half_wc_2 - 2.0,
                                                    /* a2 */ tan_half_wc_2 - tan_half_wc / gain + 1.0);
                    break;
                }
                case filterTypeBandPass: {
                    coefficients = IIRCoefficients (/* b0 */ tan_half_bw,
                                                    /* b1 */ 0.0,
                                                    /* b2 */ -tan_half_bw,
                                                    /* a0 */ 1.0 + tan_half_bw,
                                                    /* a1 */ two_cos_wc,
                                                    /* a2 */ 1.0 - tan_half_bw);
                    break;
                }
                case filterTypePeakingNotch: {
                    coefficients = IIRCoefficients (/* b0 */ sqrt_gain + gain * tan_half_bw,
                                                    /* b1 */ sqrt_gain * two_cos_wc,
                                                    /* b2 */ sqrt_gain - gain * tan_half_bw,
                                                    /* a0 */ sqrt_gain + tan_half_bw,
                                                    /* a1 */ sqrt_gain * two_cos_wc,
                                                    /* a2 */ sqrt_gain - tan_half_bw);
                    break;
                }
            }

            setCoefficients (coefficients);
        }
    };

    OwnedArray<Filter> filters;
    void updateFilters();

    float centreFrequency;
    float lfoPhase;
    float inverseSampleRate;
    float twoPi;

    Array<float> envelopes;
    float inverseE;
    float calculateAttackOrRelease (float value);

    //======================================

    PluginParametersManager parameters;

    PluginParameterComboBox paramMode;
    PluginParameterLinSlider paramMix;
    PluginParameterLogSlider paramFrequency;
    PluginParameterLinSlider paramQfactor;
    PluginParameterLinSlider paramGain;
    PluginParameterComboBox paramFilterType;
    PluginParameterLinSlider paramLFOfrequency;
    PluginParameterLinSlider paramMixLFOandEnvelope;
    PluginParameterLinSlider paramEnvelopeAttack;
    PluginParameterLinSlider paramEnvelopeRelease;

private:
    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WahWahPlugin0104AudioProcessor)
};
