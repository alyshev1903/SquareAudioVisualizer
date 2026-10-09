#include "BandFilter.h"

void BandFilter::prepare(double newSampleRate, int maxBlockSize)
{
    sampleRate = newSampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = newSampleRate;
    spec.maximumBlockSize = (juce::uint32)maxBlockSize;
    spec.numChannels = 1;

    lowCutFilter.prepare(spec);
    lowCutFilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

    highCutFilter.prepare(spec);
    highCutFilter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);

    lowCutActive = false;
    highCutActive = false;
}

void BandFilter::setCutoffs(float lowCutHz, float highCutHz)
{
    const bool lowNowActive = lowCutHz > (float)Parameters::minFreqAudible + 0.5f;
    const bool highNowActive = highCutHz < (float)Parameters::maxFreqAudible - 0.5f;

    if (lowNowActive && !lowCutActive)
        lowCutFilter.reset();
    if (highNowActive && !highCutActive)
        highCutFilter.reset();

    lowCutActive = lowNowActive;
    highCutActive = highNowActive;

    const float limit = (float)(0.45 * sampleRate);
    const float low = juce::jmin(lowCutHz, limit);
    const float high = juce::jmin(juce::jmax(highCutHz, low * 1.25f), limit);

    lowCutFilter.setCutoffFrequency(low);
    highCutFilter.setCutoffFrequency(high);
}

void BandFilter::process(float* samples, int numSamples)
{
    if (!lowCutActive && !highCutActive)
        return;

    for (int s = 0; s < numSamples; ++s)
    {
        float x = samples[s];
        if (lowCutActive)
            x = lowCutFilter.processSample(0, x);
        if (highCutActive)
            x = highCutFilter.processSample(0, x);
        samples[s] = x;
    }
}