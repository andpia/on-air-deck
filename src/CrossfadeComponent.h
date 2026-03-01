#pragma once
#include <JuceHeader.h>
#include "Theme.h"

class CrossfadeComponent : public juce::Component
{
public:
    CrossfadeComponent()
    {
        crossfadeSlider.setRange(0.0, 100.0);
        crossfadeSlider.setValue(50.0);
        crossfadeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        crossfadeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(crossfadeSlider);
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(Theme::bgCard);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 12.0f);

        const int pad = 20;

        g.setColour(Theme::textPrimary);
        g.setFont(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), 16.0f, juce::Font::bold));
        g.drawText("Crossfade", pad, pad, 200, 22, juce::Justification::centredLeft);

        // Labels
        const int labelY = getHeight() - 26;
        g.setFont(juce::FontOptions(13.0f));

        g.setColour(Theme::accentPurple);
        g.drawText("Music", pad, labelY, 80, 18, juce::Justification::centredLeft);

        g.setColour(Theme::textSecondary);
        g.drawText("Balanced", 0, labelY, getWidth(), 18, juce::Justification::centred);

        g.setColour(Theme::accentPink);
        g.drawText("Voice", 0, labelY, getWidth() - pad, 18, juce::Justification::centredRight);
    }

    void resized() override
    {
        const int pad = 20;
        crossfadeSlider.setBounds(pad, 44, getWidth() - pad * 2, 30);
    }

private:
    juce::Slider crossfadeSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CrossfadeComponent)
};
