#include "SidebarComponent.h"

using namespace OnAirDeck;

SidebarComponent::SidebarComponent()
{
    setOpaque (true);
}

void SidebarComponent::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds();
    const int  pad    = Theme::panelPadding;

    // Background
    g.fillAll (Theme::sidebarBg());

    // Right-side separator border
    g.setColour (Theme::borderCol());
    g.fillRect (bounds.getRight() - 1, bounds.getY(), 1, bounds.getHeight());

    // App name / logo
    g.setFont (juce::FontOptions (18.0f).withStyle ("Bold"));
    g.setColour (Theme::accentColour());
    g.drawText ("OnAirDeck",
                pad, pad, bounds.getWidth() - 2 * pad, 28,
                juce::Justification::centredLeft, true);

    // Navigation items
    const char* const navItems[] = { "Playlist", "Streams", "Voice-Over", "Settings" };
    const int itemHeight = 36;
    const int startY     = pad + 28 + 16;

    g.setFont (juce::FontOptions (14.0f));

    for (int i = 0; i < 4; ++i)
    {
        const int itemY = startY + i * (itemHeight + 4);

        // Highlight first item as "active"
        if (i == 0)
        {
            g.setColour (Theme::btnSecCol());
            g.fillRoundedRectangle (static_cast<float> (pad),
                                    static_cast<float> (itemY),
                                    static_cast<float> (bounds.getWidth() - 2 * pad),
                                    static_cast<float> (itemHeight),
                                    Theme::cornerRadius);
        }

        g.setColour (i == 0 ? Theme::textPrim() : Theme::textSec());
        g.drawText (navItems[i],
                    pad * 2, itemY, bounds.getWidth() - 3 * pad, itemHeight,
                    juce::Justification::centredLeft, true);
    }
}

void SidebarComponent::resized()
{
}
