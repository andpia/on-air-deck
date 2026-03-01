#pragma once
#include <JuceHeader.h>
#include "Theme.h"

class SidebarComponent : public juce::Component
{
public:
    SidebarComponent()
    {
        for (int i = 0; i < numNav; ++i)
        {
            navButtons[i].setButtonText(navLabels[i]);
            navButtons[i].setClickingTogglesState(false);
            navButtons[i].onClick = [this, i] { selectedIndex = i; repaint(); };
            addAndMakeVisible(navButtons[i]);
        }
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(Theme::bgSidebar);

        // Logo area
        const int logoH = 80;
        const juce::Rectangle<float> logoBounds(0.0f, 0.0f, (float)getWidth(), (float)logoH);

        // Purple circle
        const float cx = 36.0f, cy = (float)logoH / 2.0f, cr = 18.0f;
        g.setColour(Theme::accentPurple);
        g.fillEllipse(cx - cr, cy - cr, cr * 2.0f, cr * 2.0f);

        // Music note inside circle
        g.setColour(Theme::textPrimary);
        g.setFont(juce::FontOptions(14.0f));
        g.drawText(juce::String::fromUTF8("\xe2\x99\xaa"),
                   juce::Rectangle<int>((int)(cx - cr), (int)(cy - cr), (int)(cr * 2.0f), (int)(cr * 2.0f)),
                   juce::Justification::centred, false);

        // App name
        g.setColour(Theme::textPrimary);
        g.setFont(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), 18.0f, juce::Font::bold));
        g.drawText("OnAirDeck",
                   juce::Rectangle<int>(64, 0, getWidth() - 70, logoH),
                   juce::Justification::centredLeft, true);

        // Separator
        g.setColour(Theme::textMuted);
        g.drawHorizontalLine(logoH, 16.0f, (float)(getWidth() - 16));

        // Nav button backgrounds & labels
        for (int i = 0; i < numNav; ++i)
        {
            const auto& b = navButtons[i].getBounds();
            const bool sel = (i == selectedIndex);

            if (sel)
                g.setColour(Theme::bgSelected);
            else if (navHover == i)
                g.setColour(Theme::bgCardHover);
            else
                g.setColour(juce::Colours::transparentBlack);

            g.fillRoundedRectangle(b.toFloat().reduced(4.0f, 2.0f), 8.0f);

            // Icon placeholder dot
            const float iconX = (float)b.getX() + 20.0f;
            const float iconY = (float)b.getCentreY();
            g.setColour(sel ? Theme::accentPurple : Theme::textSecondary);
            g.fillEllipse(iconX - 5.0f, iconY - 5.0f, 10.0f, 10.0f);

            // Label
            g.setColour(sel ? Theme::textPrimary : Theme::textSecondary);
            g.setFont(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), 15.0f,
                                        sel ? juce::Font::bold : juce::Font::plain));
            g.drawText(navLabels[i],
                       b.withLeft(b.getX() + 42).withRight(b.getRight() - 8),
                       juce::Justification::centredLeft, true);
        }
    }

    void resized() override
    {
        const int logoH = 80;
        const int btnH = 48;
        const int startY = logoH + 16;
        for (int i = 0; i < numNav; ++i)
            navButtons[i].setBounds(8, startY + i * (btnH + 4), getWidth() - 16, btnH);
    }

private:
    static constexpr int numNav = 5;
    const char* navLabels[numNav] = { "Dashboard", "Library", "Playlists", "Schedule", "Statistics" };
    juce::TextButton navButtons[numNav];
    int selectedIndex = 0;
    int navHover = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SidebarComponent)
};
