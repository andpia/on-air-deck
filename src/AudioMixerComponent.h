#pragma once
#include <JuceHeader.h>
#include "Theme.h"

class AudioMixerComponent : public juce::Component
{
public:
    AudioMixerComponent()
    {
        for (int i = 0; i < 2; ++i)
        {
            faders[i].setRange(0.0, 100.0);
            faders[i].setValue(i == 0 ? 80.0 : 65.0);
            faders[i].setSliderStyle(juce::Slider::LinearHorizontal);
            faders[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
            faders[i].onValueChange = [this] { repaint(); };
            addAndMakeVisible(faders[i]);

            muteButtons[i].setButtonText("M");
            muteButtons[i].setClickingTogglesState(true);
            muteButtons[i].setColour(juce::TextButton::buttonColourId, Theme::bgCardHover);
            muteButtons[i].setColour(juce::TextButton::buttonOnColourId, Theme::accentRed);
            muteButtons[i].setColour(juce::TextButton::textColourOffId, Theme::textSecondary);
            muteButtons[i].setColour(juce::TextButton::textColourOnId, Theme::textPrimary);
            addAndMakeVisible(muteButtons[i]);
        }
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(Theme::bgCard);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 12.0f);

        const int pad = 20;

        // Title
        g.setColour(Theme::textPrimary);
        g.setFont(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), 16.0f, juce::Font::bold));
        g.drawText("Audio Mixer", pad, pad, 200, 22, juce::Justification::centredLeft);

        const int stripH = 100;
        const int stripY0 = pad + 40;

        for (int i = 0; i < 2; ++i)
        {
            const int sy = stripY0 + i * (stripH + 8);
            const juce::Colour iconColor = (i == 0) ? Theme::accentPurple : Theme::accentPink;

            // Icon circle
            g.setColour(iconColor);
            g.fillEllipse((float)pad, (float)(sy + 8), 32.0f, 32.0f);

            // Channel name
            g.setColour(Theme::textPrimary);
            g.setFont(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), 14.0f, juce::Font::bold));
            g.drawText(i == 0 ? "Music" : "VoiceOver",
                       pad + 40, sy + 8, 140, 22, juce::Justification::centredLeft);

            // Level meter bar (behind slider, drawn below fader)
            const float meterW = (float)(getWidth() - pad * 2 - 40 - 48);
            const float meterX = (float)(pad + 40);
            const float meterY = (float)(sy + 58);
            const float meterH = 10.0f;
            const float level  = (float)(faders[i].getValue() / 100.0);

            g.setColour(Theme::bgCardHover);
            g.fillRoundedRectangle(meterX, meterY, meterW, meterH, 4.0f);

            juce::ColourGradient meterGrad(iconColor, meterX, meterY,
                                           iconColor.brighter(0.4f), meterX + meterW * level, meterY, false);
            g.setGradientFill(meterGrad);
            g.fillRoundedRectangle(meterX, meterY, meterW * level, meterH, 4.0f);
        }
    }

    void resized() override
    {
        const int pad = 20;
        const int stripH = 100;
        const int stripY0 = pad + 40;

        for (int i = 0; i < 2; ++i)
        {
            const int sy = stripY0 + i * (stripH + 8);
            const int faderX = pad + 40;
            const int faderW = getWidth() - faderX - pad - 48;

            faders[i].setBounds(faderX, sy + 30, faderW, 28);
            muteButtons[i].setBounds(getWidth() - pad - 40, sy + 10, 36, 36);
        }
    }

private:
    juce::Slider     faders[2];
    juce::TextButton muteButtons[2];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMixerComponent)
};
