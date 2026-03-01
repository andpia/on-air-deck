#pragma once

#include <JuceHeader.h>
#include "SidebarComponent.h"
#include "NowPlayingComponent.h"
#include "UpNextComponent.h"
#include "StreamStatusComponent.h"
#include "AudioMixerComponent.h"
#include "CrossfadeComponent.h"
#include "VolumeMeterComponent.h"

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SidebarComponent      sidebar;
    NowPlayingComponent   nowPlaying;
    UpNextComponent       upNext;
    StreamStatusComponent streamStatus;
    AudioMixerComponent   audioMixer;
    CrossfadeComponent    crossfade;
    VolumeMeterComponent  volumeMeters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};