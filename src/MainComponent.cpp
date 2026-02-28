#include "MainComponent.h"

using namespace OnAirDeck;

MainComponent::MainComponent()
{
    addAndMakeVisible (sidebarComponent);
    addAndMakeVisible (contentComponent);

    setSize (960, 600);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (Theme::windowBg());
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    sidebarComponent.setBounds (area.removeFromLeft (Theme::sidebarWidth));
    contentComponent.setBounds (area);
}