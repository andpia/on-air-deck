#pragma once
#include <JuceHeader.h>
#include "Theme.h"

class StreamStatusComponent : public juce::Component,
                              public juce::Timer
{
public:
    StreamStatusComponent()
    {
        static constexpr int initialHours   = 14;
        static constexpr int initialMinutes = 32;
        static constexpr int initialSeconds = 18;
        uptimeSeconds = initialHours * 3600 + initialMinutes * 60 + initialSeconds;
        startTimer(1000);
    }

    ~StreamStatusComponent() override
    {
        stopTimer();
    }

    void timerCallback() override
    {
        ++uptimeSeconds;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(Theme::bgCard);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 12.0f);

        const int pad = 20;

        // Green dot + LIVE label
        g.setColour(Theme::accentGreen);
        g.fillEllipse((float)pad, (float)(getHeight() / 2 - 8), 14.0f, 14.0f);

        g.setFont(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), 14.0f, juce::Font::bold));
        g.drawText("LIVE", pad + 22, getHeight() / 2 - 10, 50, 20, juce::Justification::centredLeft);

        // Listener count
        g.setColour(Theme::textPrimary);
        g.setFont(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), 42.0f, juce::Font::bold));
        g.drawText("247", pad, 16, 120, 52, juce::Justification::centredLeft);

        g.setColour(Theme::textSecondary);
        g.setFont(juce::FontOptions(13.0f));
        g.drawText("Listeners", pad, 66, 100, 18, juce::Justification::centredLeft);

        // Bitrate
        g.setColour(Theme::textPrimary);
        g.setFont(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), 18.0f, juce::Font::bold));
        g.drawText("320 kbps", getWidth() / 2 - 40, 24, 160, 28, juce::Justification::centredLeft);

        g.setColour(Theme::textSecondary);
        g.setFont(juce::FontOptions(13.0f));
        g.drawText("Bitrate", getWidth() / 2 - 40, 54, 100, 18, juce::Justification::centredLeft);

        // Uptime
        const int hh = uptimeSeconds / 3600;
        const int mm = (uptimeSeconds % 3600) / 60;
        const int ss = uptimeSeconds % 60;
        const juce::String uptime = juce::String::formatted("%02d:%02d:%02d", hh, mm, ss);

        g.setColour(Theme::textPrimary);
        g.setFont(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), 18.0f, juce::Font::bold));
        g.drawText(uptime, getWidth() - 160, 24, 140, 28, juce::Justification::centredRight);

        g.setColour(Theme::textSecondary);
        g.setFont(juce::FontOptions(13.0f));
        g.drawText("Uptime", getWidth() - 160, 54, 140, 18, juce::Justification::centredRight);
    }

private:
    int uptimeSeconds = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StreamStatusComponent)
};
