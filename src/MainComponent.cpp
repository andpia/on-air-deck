#include "MainComponent.h"
#include "Theme.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(sidebar);
    addAndMakeVisible(nowPlaying);
    addAndMakeVisible(upNext);
    addAndMakeVisible(streamStatus);
    addAndMakeVisible(audioMixer);
    addAndMakeVisible(crossfade);
    addAndMakeVisible(volumeMeters);

    setSize(1920, 1080);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(Theme::bgDark);
}

void MainComponent::resized()
{
    sidebar.setBounds(0, 0, 230, 1080);

    nowPlaying.setBounds(230, 20, 940, 520);
    upNext.setBounds(230, 560, 940, 500);

    streamStatus.setBounds(1200, 20, 710, 140);
    audioMixer.setBounds(1200, 180, 710, 300);
    crossfade.setBounds(1200, 500, 710, 120);
    volumeMeters.setBounds(1200, 640, 710, 180);
}
