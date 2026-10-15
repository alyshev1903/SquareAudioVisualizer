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

BandFilter::Cutoffs BandFilter::resolveCutoffs(float lowCutHz, float highCutHz, double sampleRate)
{
    Cutoffs cutoffs;
    cutoffs.lowActive = lowCutHz > (float)Parameters::minFreqAudible + 0.5f;
    cutoffs.highActive = highCutHz < (float)Parameters::maxFreqAudible - 0.5f;

    const float limit = (float)(0.45 * sampleRate);
    cutoffs.low = juce::jmin(lowCutHz, limit);
    cutoffs.high = juce::jmin(juce::jmax(highCutHz, cutoffs.low * 1.25f), limit);
    return cutoffs;
}

void BandFilter::setCutoffs(float lowCutHz, float highCutHz)
{
    const auto cutoffs = resolveCutoffs(lowCutHz, highCutHz, sampleRate);

    if (cutoffs.lowActive && !lowCutActive)
        lowCutFilter.reset();
    if (cutoffs.highActive && !highCutActive)
        highCutFilter.reset();

    lowCutActive = cutoffs.lowActive;
    highCutActive = cutoffs.highActive;

    lowCutFilter.setCutoffFrequency(cutoffs.low);
    highCutFilter.setCutoffFrequency(cutoffs.high);
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

float BandFilter::magnitudeDb(float hz, const Cutoffs& cutoffs)
{
    float magnitude = 1.0f;

    if (cutoffs.lowActive)
    {
        const float x = hz / cutoffs.low;
        const float x4 = x * x * x * x;
        magnitude *= x4 / (1.0f + x4);
    }

    if (cutoffs.highActive)
    {
        const float x = hz / cutoffs.high;
        const float x4 = x * x * x * x;
        magnitude *= 1.0f / (1.0f + x4);
    }

    return 20.0f * std::log10(juce::jmax(magnitude, 1.0e-4f));
}