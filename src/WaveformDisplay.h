#pragma once
#include <JuceHeader.h>
#include "Theme.h"

class WaveformDisplay : public juce::Component
{
public:
    WaveformDisplay()
    {
        juce::Random rng;
        for (auto& h : barHeights)
            h = 0.15f + rng.nextFloat() * 0.85f;
    }

    void setPosition(float pos)
    {
        playheadPosition = juce::jlimit(0.0f, 1.0f, pos);
        repaint();
    }

    float getPosition() const { return playheadPosition; }

    void paint(juce::Graphics& g) override
    {
        const int w = getWidth();
        const int h = getHeight();
        const float barWidth = (float)w / numBars;
        const float playheadX = playheadPosition * (float)w;

        for (int i = 0; i < numBars; ++i)
        {
            const float bx = (float)i * barWidth;
            const float bh = barHeights[i] * (float)h * 0.9f;
            const float by = ((float)h - bh) / 2.0f;

            g.setColour(bx < playheadX ? Theme::accentOrange : Theme::accentPurple);
            g.fillRoundedRectangle(bx + 1.0f, by, barWidth - 2.0f, bh, 2.0f);
        }

        // Playhead line
        g.setColour(juce::Colours::white);
        g.drawLine(playheadX, 0.0f, playheadX, (float)h, 2.0f);

        // Circle handle at top
        const float circleR = 6.0f;
        g.fillEllipse(playheadX - circleR, 0.0f, circleR * 2.0f, circleR * 2.0f);
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        setPosition((float)e.x / (float)getWidth());
    }

    void mouseDrag(const juce::MouseEvent& e) override
    {
        setPosition((float)e.x / (float)getWidth());
    }

private:
    static constexpr int numBars = 120;
    static constexpr float initialPlayheadPosition = 0.38f; // ~2:36 of 4:04 elapsed

    float barHeights[numBars] {};
    float playheadPosition = initialPlayheadPosition;
};
