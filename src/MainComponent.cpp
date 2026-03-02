#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible (webUI);
    setSize (1280, 800);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (juce::Graphics& g)
{
    // Fill the background; the WebUIComponent covers the entire area.
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    webUI.setBounds (getLocalBounds());
}