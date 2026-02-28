#pragma once

#include <JuceHeader.h>
#include "SidebarComponent.h"
#include "ContentComponent.h"
#include "ThemeTokens.h"

/**
    @class MainComponent
    @brief The main content component of the application.

    Implements the RootLayout concept from the Figma design:
    a fixed-width sidebar on the left and a content area that
    fills the remaining space on the right.
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
        Draws the component background using the dark theme window token.
        @param g The graphics context used for drawing.
    */
    void paint (juce::Graphics&) override;

    /**
        Lays out the sidebar and content child components.
    */
    void resized() override;

private:
    SidebarComponent sidebarComponent;
    ContentComponent contentComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};