#pragma once

#include <JuceHeader.h>
#include "Parameters.h"
#include "SharedAudioData.h"
#include "BandFilter.h"
#include "FFTProcessor.h"

class FilterDisplay : public juce::Component, private juce::Timer
{
public:
    FilterDisplay(AudioProcessorValueTreeState& state, SharedAudioData& sharedData);
    ~FilterDisplay() override;

    void setAccent(juce::Colour newAccent);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    float xForFrequency(float hz) const;

    AudioProcessorValueTreeState& apvts;
    SharedAudioData& sharedAudioData;

    std::vector<float> spectrum;
    std::vector<float> bands;
    juce::Rectangle<float> plotArea;
    juce::Colour accent = juce::Colours::teal;
    int numBands = 128;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterDisplay)
};
