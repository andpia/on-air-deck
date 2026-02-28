#include "ContentComponent.h"

using namespace OnAirDeck;

ContentComponent::ContentComponent()
{
    setOpaque (true);
}

void ContentComponent::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds();
    const int  pad    = Theme::panelPadding;

    // Background
    g.fillAll (Theme::panelBg());

    // Placeholder card to demonstrate bgCard token
    const int cardMargin = pad * 2;
    const auto cardBounds = bounds.reduced (cardMargin).withHeight (120);

    g.setColour (Theme::cardBg());
    g.fillRoundedRectangle (cardBounds.toFloat(), Theme::cornerRadius);

    g.setColour (Theme::borderCol());
    g.drawRoundedRectangle (cardBounds.toFloat(), Theme::cornerRadius, 1.0f);

    g.setFont (juce::FontOptions (13.0f).withStyle ("Bold"));
    g.setColour (Theme::textPrim());
    g.drawText ("Now Playing",
                cardBounds.getX() + pad,
                cardBounds.getY() + pad,
                200, 20,
                juce::Justification::centredLeft, true);

    g.setFont (juce::FontOptions (12.0f));
    g.setColour (Theme::textSec());
    g.drawText ("No track loaded",
                cardBounds.getX() + pad,
                cardBounds.getY() + pad + 24,
                cardBounds.getWidth() - 2 * pad, 18,
                juce::Justification::centredLeft, true);

    // Footer hint
    g.setFont (juce::FontOptions (11.0f));
    g.setColour (Theme::textDis());
    g.drawText ("OnAirDeck \xe2\x80\x94 Professional Web Radio Engine",
                pad, bounds.getBottom() - 24, bounds.getWidth() - 2 * pad, 18,
                juce::Justification::centredLeft, true);
}

void ContentComponent::resized()
{
}
