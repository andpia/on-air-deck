#pragma once

#include <JuceHeader.h>
#include "ThemeTokens.h"

/**
    @class DarkLookAndFeel
    @brief Custom JUCE LookAndFeel implementing the OnAirDeck dark theme.

    Inherits from juce::LookAndFeel_V4 and overrides drawing methods and colour
    IDs so that all standard JUCE widgets automatically adopt the design tokens
    defined in ThemeTokens.h.

    Usage:
    @code
        // In your application initialise():
        juce::LookAndFeel::setDefaultLookAndFeel (&darkLookAndFeel);
    @endcode
*/
class DarkLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DarkLookAndFeel();
    ~DarkLookAndFeel() override = default;

    //==============================================================================
    // Button overrides
    void drawButtonBackground (juce::Graphics&,
                               juce::Button&,
                               const juce::Colour& backgroundColour,
                               bool isMouseOverButton,
                               bool isButtonDown) override;

    void drawButtonText (juce::Graphics&,
                         juce::TextButton&,
                         bool isMouseOverButton,
                         bool isButtonDown) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DarkLookAndFeel)
};
