#pragma once

#include <JuceHeader.h>

/**
    @class MainComponent
    @brief The main content component of the application.

    This component lives inside the main window and contains all the
    application's controls and content.
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
        Draws the content of the component.
        @param g The graphics context used for drawing.
    */
    void paint (juce::Graphics&) override;

    /**
        Called when the component is resized.
        Use this method to layout child components.
    */
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};