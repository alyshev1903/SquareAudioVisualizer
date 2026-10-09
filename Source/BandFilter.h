#pragma once

#include <JuceHeader.h>
#include "Parameters.h"

class BandFilter
{
public:
    BandFilter() = default;

    void prepare(double newSampleRate, int maxBlockSize);
    void setCutoffs(float lowCutHz, float highCutHz);
    void process(float* samples, int numSamples);

private:
    juce::dsp::LinkwitzRileyFilter<float> lowCutFilter;
    juce::dsp::LinkwitzRileyFilter<float> highCutFilter;

    double sampleRate = 44100.0;
    bool   lowCutActive = false;
    bool   highCutActive = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandFilter)
};