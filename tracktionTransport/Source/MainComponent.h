#pragma once

#include <JuceHeader.h>
using namespace juce;
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  :  public AudioAppComponent,
                        public Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    void play();
    void stop();
    void open();
    void timerCallback() override;
    
private:
    tracktion_engine::Engine engine { ProjectInfo::projectName };
    std::unique_ptr<tracktion_engine::Edit> edit;
    
    enum PlayState{
        Play,
        Stop
    };
    PlayState playState { PlayState::Stop };
    
    TextButton playButton { "Play" };
    TextButton stopButton { "Stop" };
    TextButton openButton { "Open" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
