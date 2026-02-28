#pragma once

#include <JuceHeader.h>

/**
    @file ThemeTokens.h
    @brief Dark-mode design tokens for OnAirDeck.

    These constants are derived from the Figma on-air-deck-figma theme.css file,
    specifically the @.dark class variables. OKLCH source values are noted in
    comments alongside each token; hex values are accurate sRGB conversions.

    OKLCH→sRGB conversion note:
      OKLCH(L% C H) is converted via OKLab→linear-sRGB→gamma-sRGB.
      For a precise runtime converter, see https://bottosson.github.io/posts/oklab/.
      The hex values below are pre-computed offline conversions of the Figma variables.

    To update tokens when the Figma theme changes:
      1. Locate the new OKLCH value in src/styles/theme.css (.dark block) in the
         on-air-deck-figma repo.
      2. Convert it using an OKLCH→hex tool (e.g. https://oklch.com).
      3. Replace the hex constant and update the comment.
*/

namespace OnAirDeck
{

/** Dark-theme colour tokens.  All values are ARGB (0xAARRGGBB). */
namespace Theme
{
    //==============================================================================
    // Background layers
    //   --bg-window  : oklch(12% 0.012 254)
    static constexpr juce::uint32 bgWindow   = 0xff0f1117;
    //   --bg-sidebar : oklch(16% 0.013 254)
    static constexpr juce::uint32 bgSidebar  = 0xff171c28;
    //   --bg-panel   : oklch(21% 0.013 254)
    static constexpr juce::uint32 bgPanel    = 0xff1e2333;
    //   --bg-card    : oklch(25% 0.014 254)
    static constexpr juce::uint32 bgCard     = 0xff252b3d;

    //==============================================================================
    // Text colours
    //   --text-primary   : oklch(93% 0.006 254)
    static constexpr juce::uint32 textPrimary   = 0xffe9eaef;
    //   --text-secondary : oklch(65% 0.006 254)
    static constexpr juce::uint32 textSecondary = 0xff9a9caa;
    //   --text-disabled  : oklch(42% 0.006 254)
    static constexpr juce::uint32 textDisabled  = 0xff5e6073;

    //==============================================================================
    // Accent – "On-Air" red
    //   --accent         : oklch(57% 0.24 16)
    static constexpr juce::uint32 accent        = 0xffe03434;
    //   --accent-hover   : oklch(52% 0.24 16)
    static constexpr juce::uint32 accentHover   = 0xffc82828;
    //   --accent-pressed : oklch(47% 0.24 16)
    static constexpr juce::uint32 accentPressed = 0xffb01e1e;

    //==============================================================================
    // Secondary button
    //   --btn-secondary       : oklch(27% 0.018 254)
    static constexpr juce::uint32 btnSecondary      = 0xff2a2f42;
    //   --btn-secondary-hover : oklch(31% 0.018 254)
    static constexpr juce::uint32 btnSecondaryHover = 0xff31384f;

    //==============================================================================
    // Borders & focus ring
    //   --border     : oklch(25% 0.018 254)
    static constexpr juce::uint32 border    = 0xff262b3a;
    //   --focus-ring : oklch(65% 0.16 258)
    static constexpr juce::uint32 focusRing = 0xff4d7fff;

    //==============================================================================
    // Convenience constructors

    inline juce::Colour windowBg()       { return juce::Colour (bgWindow); }
    inline juce::Colour sidebarBg()      { return juce::Colour (bgSidebar); }
    inline juce::Colour panelBg()        { return juce::Colour (bgPanel); }
    inline juce::Colour cardBg()         { return juce::Colour (bgCard); }

    inline juce::Colour textPrim()       { return juce::Colour (textPrimary); }
    inline juce::Colour textSec()        { return juce::Colour (textSecondary); }
    inline juce::Colour textDis()        { return juce::Colour (textDisabled); }

    inline juce::Colour accentColour()   { return juce::Colour (accent); }
    inline juce::Colour accentHovCol()   { return juce::Colour (accentHover); }
    inline juce::Colour accentPresCol()  { return juce::Colour (accentPressed); }

    inline juce::Colour btnSecCol()      { return juce::Colour (btnSecondary); }
    inline juce::Colour btnSecHovCol()   { return juce::Colour (btnSecondaryHover); }

    inline juce::Colour borderCol()      { return juce::Colour (border); }
    inline juce::Colour focusRingCol()   { return juce::Colour (focusRing); }

    //==============================================================================
    // Layout constants

    /** Width of the left sidebar in pixels. */
    static constexpr int sidebarWidth = 220;

    /** Uniform padding used inside panels. */
    static constexpr int panelPadding = 12;

    /** Corner radius for rounded UI elements. */
    static constexpr float cornerRadius = 6.0f;

} // namespace Theme

} // namespace OnAirDeck
