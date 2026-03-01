#pragma once
#include <JuceHeader.h>
#include "Theme.h"

class VolumeMeterComponent : public juce::Component,
                             public juce::Timer
{
public:
    VolumeMeterComponent()
    {
        levelsL = 0.46f;
        levelsR = 0.60f;
        startTimer(1000 / 24);
    }

    ~VolumeMeterComponent() override
    {
        stopTimer();
    }

    void timerCallback() override
    {
        static constexpr float baseLevelL      = 0.46f;
        static constexpr float baseLevelR      = 0.60f;
        static constexpr float fluctuationHalf = 0.05f; // ±5%
        juce::Random& rng = juce::Random::getSystemRandom();
        levelsL = juce::jlimit(0.0f, 1.0f, baseLevelL + (rng.nextFloat() - 0.5f) * fluctuationHalf * 2.0f);
        levelsR = juce::jlimit(0.0f, 1.0f, baseLevelR + (rng.nextFloat() - 0.5f) * fluctuationHalf * 2.0f);
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(Theme::bgCard);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 12.0f);

        const int pad = 20;

        g.setColour(Theme::accentGreen);
        g.setFont(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), 16.0f, juce::Font::bold));
        g.drawText("Volume Meters", pad, pad, 200, 22, juce::Justification::centredLeft);

        drawMeter(g, "L", levelsL, pad, 56);
        drawMeter(g, "R", levelsR, pad, 56 + 52);
    }

private:
    void drawMeter(juce::Graphics& g, const juce::String& label, float level, int padX, int y)
    {
        g.setColour(Theme::textSecondary);
        g.setFont(juce::FontOptions(13.0f));
        g.drawText(label, padX, y, 20, 32, juce::Justification::centred);

        const int meterX = padX + 28;
        const int meterW = getWidth() - padX * 2 - 28;
        const int meterH = 20;
        const int meterY = y + 6;

        // Track
        g.setColour(Theme::bgCardHover);
        g.fillRoundedRectangle((float)meterX, (float)meterY, (float)meterW, (float)meterH, 4.0f);

        // Fill with green-to-yellow gradient
        const float fillW = (float)meterW * level;
        juce::ColourGradient grad(Theme::meterGreen, (float)meterX, (float)meterY,
                                   Theme::meterYellow, (float)(meterX + meterW), (float)meterY, false);
        g.setGradientFill(grad);
        g.fillRoundedRectangle((float)meterX, (float)meterY, fillW, (float)meterH, 4.0f);
    }

    float levelsL = 0.46f;
    float levelsR = 0.60f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VolumeMeterComponent)
};
