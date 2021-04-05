#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    edit = std::make_unique<tracktion_engine::Edit>(engine, tracktion_engine::createEmptyEdit(engine), tracktion_engine::Edit::EditRole::forEditing, nullptr, 0);
    
    playButton.setToggleState(false, NotificationType::dontSendNotification);
    playButton.onClick = [this] { play(); };
    addAndMakeVisible(playButton);
    
    stopButton.setToggleState(true, NotificationType::dontSendNotification);
    stopButton.onClick = [this] { stop(); };
    addAndMakeVisible(stopButton);
    
    openButton.onClick = [this] { open(); };
    addAndMakeVisible(openButton);

    setSize (300, 300);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

void MainComponent::play()
{
    if(playState == PlayState::Stop)
    {
        playState = PlayState::Play;
        DBG("Play");
        edit->getTransport().play(false);
        Timer::startTimer(100);
    }
}

void MainComponent::stop()
{
    if(playState == PlayState::Play)
    {
        playState = PlayState::Stop;
        DBG("Stop");
        edit->getTransport().stop(true, false);
        Timer::stopTimer();
    }
}

void MainComponent::open()
{
    FileChooser chooser("Select File", File::getSpecialLocation(File::userDesktopDirectory),{});
    if(chooser.browseForFileToOpen())
    {
        File f = chooser.getResult();
        edit.reset(new tracktion_engine::Edit(engine, tracktion_engine::loadEditFromFile(engine, f, tracktion_engine::ProjectItemID::createNewID (0)), tracktion_engine::Edit::EditRole::forEditing, nullptr, 0));
    }
}

void MainComponent::timerCallback()
{
    DBG("Time : " <<edit->getTransport().getCurrentPosition());
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    Rectangle<int> bounds = getLocalBounds();
    
    FlexBox flexbox { FlexBox::Direction::row, FlexBox::Wrap::noWrap, FlexBox::AlignContent::stretch, FlexBox::AlignItems::stretch, FlexBox::JustifyContent::center };
    
    flexbox.items.add(FlexItem(100, 100, playButton));
    flexbox.items.add(FlexItem(100, 100, stopButton));
    flexbox.items.add(FlexItem(100, 100, openButton));
    
    flexbox.performLayout(bounds);
}
