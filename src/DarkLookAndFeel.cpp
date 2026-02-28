#include "DarkLookAndFeel.h"

using namespace OnAirDeck;

DarkLookAndFeel::DarkLookAndFeel()
{
    // ---- Backgrounds --------------------------------------------------------
    setColour (juce::ResizableWindow::backgroundColourId, Theme::windowBg());
    setColour (juce::DocumentWindow::textColourId,        Theme::textPrim());

    // ---- Labels -------------------------------------------------------------
    setColour (juce::Label::textColourId,                 Theme::textPrim());
    setColour (juce::Label::backgroundColourId,           juce::Colours::transparentBlack);
    setColour (juce::Label::outlineColourId,              juce::Colours::transparentBlack);

    // ---- Buttons ------------------------------------------------------------
    // TextButton::buttonColourId  = normal bg for accent/primary button
    // TextButton::buttonOnColourId = toggled bg
    setColour (juce::TextButton::buttonColourId,          Theme::btnSecCol());
    setColour (juce::TextButton::buttonOnColourId,        Theme::accentColour());
    setColour (juce::TextButton::textColourOffId,         Theme::textPrim());
    setColour (juce::TextButton::textColourOnId,          Theme::textPrim());

    // ---- Text editors -------------------------------------------------------
    setColour (juce::TextEditor::backgroundColourId,      Theme::cardBg());
    setColour (juce::TextEditor::textColourId,            Theme::textPrim());
    setColour (juce::TextEditor::highlightColourId,       Theme::accentColour().withAlpha (0.4f));
    setColour (juce::TextEditor::highlightedTextColourId, Theme::textPrim());
    setColour (juce::TextEditor::outlineColourId,         Theme::borderCol());
    setColour (juce::TextEditor::focusedOutlineColourId,  Theme::focusRingCol());

    // ---- ComboBox -----------------------------------------------------------
    setColour (juce::ComboBox::backgroundColourId,        Theme::cardBg());
    setColour (juce::ComboBox::textColourId,              Theme::textPrim());
    setColour (juce::ComboBox::outlineColourId,           Theme::borderCol());
    setColour (juce::ComboBox::arrowColourId,             Theme::textSec());
    setColour (juce::ComboBox::focusedOutlineColourId,    Theme::focusRingCol());

    // ---- Sliders ------------------------------------------------------------
    setColour (juce::Slider::backgroundColourId,          Theme::cardBg());
    setColour (juce::Slider::trackColourId,               Theme::accentColour());
    setColour (juce::Slider::thumbColourId,               Theme::textPrim());
    setColour (juce::Slider::textBoxTextColourId,         Theme::textPrim());
    setColour (juce::Slider::textBoxBackgroundColourId,   Theme::cardBg());
    setColour (juce::Slider::textBoxOutlineColourId,      Theme::borderCol());

    // ---- ListBox ------------------------------------------------------------
    setColour (juce::ListBox::backgroundColourId,         Theme::panelBg());
    setColour (juce::ListBox::outlineColourId,            Theme::borderCol());
    setColour (juce::ListBox::textColourId,               Theme::textPrim());

    // ---- ScrollBar ----------------------------------------------------------
    setColour (juce::ScrollBar::backgroundColourId,       juce::Colours::transparentBlack);
    setColour (juce::ScrollBar::thumbColourId,            Theme::borderCol().brighter (0.3f));
    setColour (juce::ScrollBar::trackColourId,            juce::Colours::transparentBlack);

    // ---- PopupMenu ----------------------------------------------------------
    setColour (juce::PopupMenu::backgroundColourId,       Theme::cardBg());
    setColour (juce::PopupMenu::textColourId,             Theme::textPrim());
    setColour (juce::PopupMenu::highlightedBackgroundColourId, Theme::btnSecCol());
    setColour (juce::PopupMenu::highlightedTextColourId,  Theme::textPrim());
    setColour (juce::PopupMenu::headerTextColourId,       Theme::textSec());

    // ---- ToggleButton -------------------------------------------------------
    setColour (juce::ToggleButton::textColourId,          Theme::textPrim());
    setColour (juce::ToggleButton::tickColourId,          Theme::accentColour());
    setColour (juce::ToggleButton::tickDisabledColourId,  Theme::textDis());

    // ---- AlertWindow --------------------------------------------------------
    setColour (juce::AlertWindow::backgroundColourId,     Theme::panelBg());
    setColour (juce::AlertWindow::textColourId,           Theme::textPrim());
    setColour (juce::AlertWindow::outlineColourId,        Theme::borderCol());
}

//==============================================================================
void DarkLookAndFeel::drawButtonBackground (juce::Graphics& g,
                                            juce::Button& button,
                                            const juce::Colour& /*backgroundColour*/,
                                            bool isMouseOverButton,
                                            bool isButtonDown)
{
    const bool isAccent = button.getToggleState() || button.getProperties()["accent"];

    juce::Colour fillColour;
    if (isAccent)
    {
        if (isButtonDown)       fillColour = Theme::accentPresCol();
        else if (isMouseOverButton) fillColour = Theme::accentHovCol();
        else                    fillColour = Theme::accentColour();
    }
    else
    {
        if (isButtonDown)       fillColour = Theme::btnSecCol().darker (0.2f);
        else if (isMouseOverButton) fillColour = Theme::btnSecHovCol();
        else                    fillColour = Theme::btnSecCol();
    }

    auto bounds = button.getLocalBounds().toFloat().reduced (0.5f);
    g.setColour (fillColour);
    g.fillRoundedRectangle (bounds, Theme::cornerRadius);

    // Border
    g.setColour (Theme::borderCol());
    g.drawRoundedRectangle (bounds, Theme::cornerRadius, 1.0f);
}

void DarkLookAndFeel::drawButtonText (juce::Graphics& g,
                                      juce::TextButton& button,
                                      bool /*isMouseOverButton*/,
                                      bool /*isButtonDown*/)
{
    g.setColour (Theme::textPrim());
    g.setFont (juce::FontOptions (14.0f));
    g.drawFittedText (button.getButtonText(),
                      button.getLocalBounds(),
                      juce::Justification::centred,
                      1);
}


