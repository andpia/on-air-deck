#pragma once
#include <JuceHeader.h>
#include "Theme.h"
#include "WaveformDisplay.h"

class NowPlayingComponent : public juce::Component
{
public:
    NowPlayingComponent()
    {
        addAndMakeVisible(waveform);

        volumeSlider.setRange(0.0, 100.0);
        volumeSlider.setValue(75.0);
        volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(volumeSlider);

        pauseButton.setButtonText("||");
        pauseButton.setColour(juce::TextButton::buttonColourId, Theme::accentPurple);
        pauseButton.setColour(juce::TextButton::textColourOffId, Theme::textPrimary);
        addAndMakeVisible(pauseButton);

        nextButton.setButtonText(">|");
        nextButton.setColour(juce::TextButton::buttonColourId, Theme::bgCardHover);
        nextButton.setColour(juce::TextButton::textColourOffId, Theme::textPrimary);
        addAndMakeVisible(nextButton);
    }

    void paint(juce::Graphics& g) override
    {
        // Card background
        g.setColour(Theme::bgCard);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 12.0f);

        const int pad = 20;

        // "Now Playing" label
        g.setFont(juce::FontOptions(13.0f));
        g.setColour(Theme::textSecondary);
        g.drawText("Now Playing", pad, pad, 200, 20, juce::Justification::centredLeft);

        // Album art area
        const juce::Rectangle<float> art(pad, pad + 28, 120.0f, 120.0f);
        juce::ColourGradient grad(Theme::accentPink, art.getX(), art.getY(),
                                  Theme::accentPurple, art.getRight(), art.getBottom(), false);
        g.setGradientFill(grad);
        g.fillRoundedRectangle(art, 10.0f);

        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.setFont(juce::FontOptions(36.0f));
        g.drawText(juce::String::fromUTF8("\xe2\x99\xaa"), art.toNearestInt(), juce::Justification::centred, false);

        // Track info
        const int infoX = pad + 136;
        const int infoY = pad + 28;

        g.setColour(Theme::textPrimary);
        g.setFont(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), 26.0f, juce::Font::bold));
        g.drawText("Midnight City", infoX, infoY, getWidth() - infoX - pad, 36, juce::Justification::centredLeft);

        g.setFont(juce::FontOptions(16.0f));
        g.setColour(Theme::textPrimary);
        g.drawText("M83", infoX, infoY + 40, getWidth() - infoX - pad, 22, juce::Justification::centredLeft);

        g.setFont(juce::FontOptions(14.0f));
        g.setColour(Theme::textSecondary);
        g.drawText("Hurry Up, We're Dreaming", infoX, infoY + 66, getWidth() - infoX - pad, 20, juce::Justification::centredLeft);

        // Time labels below waveform
        const int waveY = pad + 168;
        const int timeY = waveY + 54;

        g.setFont(juce::FontOptions(12.0f));
        g.setColour(Theme::textSecondary);
        g.drawText("ELAPSED  2:36", pad, timeY, 140, 18, juce::Justification::centredLeft);

        g.setColour(Theme::accentOrange);
        g.drawText("INTRO  0:15", getWidth() / 2 - 60, timeY, 120, 18, juce::Justification::centred);

        g.setColour(Theme::textSecondary);
        g.drawText("REMAINING  -1:28", getWidth() - pad - 150, timeY, 150, 18, juce::Justification::centredRight);

        // Total duration
        g.setColour(Theme::textMuted);
        g.setFont(juce::FontOptions(12.0f));
        g.drawText("TOTAL DURATION  4:04", 0, timeY + 22, getWidth(), 18, juce::Justification::centred);

        // Volume label
        const int volY = timeY + 54;
        g.setColour(Theme::textSecondary);
        g.setFont(juce::FontOptions(18.0f));
        g.drawText(juce::String::fromUTF8("\xf0\x9f\x94\x8a"), pad, volY + 4, 28, 28, juce::Justification::centred);
    }

    void resized() override
    {
        const int pad = 20;
        const int waveY = pad + 168;

        waveform.setBounds(pad, waveY, getWidth() - pad * 2, 50);

        const int timeY = waveY + 54;
        const int transportY = timeY + 48;

        pauseButton.setBounds(getWidth() / 2 - 50, transportY, 44, 44);
        nextButton.setBounds(getWidth() / 2 + 10, transportY, 44, 44);

        const int volY = timeY + 54;
        volumeSlider.setBounds(pad + 34, volY, getWidth() - pad * 2 - 34, 32);
    }

private:
    WaveformDisplay waveform;
    juce::Slider volumeSlider;
    juce::TextButton pauseButton, nextButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NowPlayingComponent)
};
