#pragma once

#include <JuceHeader.h>
#include "ThemeTokens.h"

/**
    @class SidebarComponent
    @brief Left-side navigation panel for the OnAirDeck main window.

    Displays the application name and a set of navigation items.
    Painted entirely using the dark theme sidebar token (ThemeTokens::bgSidebar).
*/
class SidebarComponent : public juce::Component
{
public:
    SidebarComponent();
    ~SidebarComponent() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SidebarComponent)
};
