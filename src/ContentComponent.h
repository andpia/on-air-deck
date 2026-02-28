#pragma once

#include <JuceHeader.h>
#include "ThemeTokens.h"

/**
    @class ContentComponent
    @brief Main content area of the OnAirDeck main window.

    Occupies the right portion of the window to the right of SidebarComponent.
    Painted with the dark panel background token (ThemeTokens::bgPanel).
    Future child components (playlist, stream controls, etc.) live here.
*/
class ContentComponent : public juce::Component
{
public:
    ContentComponent();
    ~ContentComponent() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ContentComponent)
};
