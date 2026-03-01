#pragma once
#include <JuceHeader.h>
#include "Theme.h"

class UpNextComponent : public juce::Component
{
public:
    UpNextComponent() {}

    void paint(juce::Graphics& g) override
    {
        g.setColour(Theme::bgCard);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 12.0f);

        const int pad = 20;

        // Header
        g.setColour(Theme::textPrimary);
        g.setFont(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), 18.0f, juce::Font::bold));
        g.drawText("Up Next", pad, pad, 150, 24, juce::Justification::centredLeft);

        g.setColour(Theme::textSecondary);
        g.setFont(juce::FontOptions(13.0f));
        g.drawText("5 tracks", pad + 160, pad, 80, 24, juce::Justification::centredLeft);

        // Track rows
        const int rowH = 52;
        const int rowsStart = pad + 44;

        for (int i = 0; i < numTracks; ++i)
        {
            const int ry = rowsStart + i * rowH;

            if (i == hoveredRow)
            {
                g.setColour(Theme::bgCardHover);
                g.fillRoundedRectangle((float)pad, (float)ry, (float)(getWidth() - pad * 2), (float)(rowH - 4), 6.0f);
            }

            // Drag handle
            g.setColour(Theme::textMuted);
            g.setFont(juce::FontOptions(14.0f));
            g.drawText("::", pad + 4, ry, 20, rowH - 4, juce::Justification::centred);

            // Track number
            g.setColour(Theme::textSecondary);
            g.setFont(juce::FontOptions(13.0f));
            g.drawText(juce::String(i + 1), pad + 28, ry, 20, rowH - 4, juce::Justification::centred);

            // Title
            g.setColour(Theme::textPrimary);
            g.setFont(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), 15.0f, juce::Font::bold));
            g.drawText(trackTitles[i], pad + 54, ry, 240, 22, juce::Justification::centredLeft);

            // Artist
            g.setColour(Theme::textSecondary);
            g.setFont(juce::FontOptions(13.0f));
            g.drawText(trackArtists[i], pad + 54, ry + 22, 240, 18, juce::Justification::centredLeft);

            // Duration
            g.setColour(Theme::textSecondary);
            g.setFont(juce::FontOptions(13.0f));
            g.drawText(trackDurations[i], getWidth() - pad - 60, ry, 60, rowH - 4, juce::Justification::centredRight);
        }
    }

    void mouseMove(const juce::MouseEvent& e) override
    {
        const int pad = 20;
        const int rowH = 52;
        const int rowsStart = pad + 44;
        int newHover = -1;
        for (int i = 0; i < numTracks; ++i)
        {
            const int ry = rowsStart + i * rowH;
            if (e.y >= ry && e.y < ry + rowH)
            {
                newHover = i;
                break;
            }
        }
        if (newHover != hoveredRow)
        {
            hoveredRow = newHover;
            repaint();
        }
    }

    void mouseExit(const juce::MouseEvent&) override
    {
        hoveredRow = -1;
        repaint();
    }

private:
    static constexpr int numTracks = 4;
    const char* trackTitles[numTracks]    = { "Breathe",  "Teardrop",       "Night Owl",  "Intro"   };
    const char* trackArtists[numTracks]   = { "Pink Floyd", "Massive Attack", "Galimatias", "The xx"  };
    const char* trackDurations[numTracks] = { "2:43",     "5:10",           "3:45",       "2:11"    };
    int hoveredRow = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UpNextComponent)
};
