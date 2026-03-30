#pragma once

#include <JuceHeader.h>
#include "WebUIComponent.hpp"

/**
    @class MainComponent
    @brief The main content component of the application.

    This component lives inside the main window and contains all the
    application's controls and content.  The entire UI surface is handed off
    to WebUIComponent, which embeds the On Air Deck Web UI via
    WebBrowserComponent.
*/
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    /** Constructor. */
    MainComponent();

    /** Destructor. */
    ~MainComponent() override;

    //==============================================================================
    /**
        Draws the background of the component.
        @param g The graphics context used for drawing.
    */
    void paint (juce::Graphics&) override;

    /**
        Called when the component is resized.
        Fills the entire bounds with the embedded WebUIComponent.
    */
    void resized() override;

private:
    WebUIComponent webUI;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};