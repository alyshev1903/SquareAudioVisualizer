#include "FFTProcessor.h"
#include <cmath>

FFTProcessor::FFTProcessor() :
    forwardFFT(fftOrder),
    window(fftSize, juce::dsp::WindowingFunction<float>::WindowingMethod::hann, false)
{}

FFTProcessor::~FFTProcessor() {}

void FFTProcessor::pushIntoFifo(float sample) noexcept
{
    if (fifoIndex == fftSize)
    {
        if (!nextFFTBlockReady)
        {
            juce::zeromem(fftData, sizeof(fftData));
            memcpy(fftData, fifo, sizeof(fifo));
            nextFFTBlockReady = true;
        }
        fifoIndex = 0;
    }

    fifo[(size_t)fifoIndex++] = sample;
}

void FFTProcessor::computeFFT()
{
    window.multiplyWithWindowingTable(fftData, fftSize);
    forwardFFT.performFrequencyOnlyForwardTransform(fftData, false);
}

float* FFTProcessor::getFFTData()
{
    return fftData;
}

bool FFTProcessor::getNextFFTBlockReady()
{
    return nextFFTBlockReady;
}

int FFTProcessor::getFFTSize()
{
    return fftSize;
}

void FFTProcessor::resetFFTBlock()
{
    nextFFTBlockReady = false;
}

std::vector<float> FFTProcessor::computeBands(const float* magnitudes,
    double sampleRate,
    int numBands,
    float minFreqHz,
    float maxFreqHz)
{
    std::vector<float> bands((size_t)juce::jmax(1, numBands), 0.0f);
    if (magnitudes == nullptr || sampleRate <= 0.0)
        return bands;

    const int   numBins = fftSize / 2;
    const float binWidth = (float)sampleRate / (float)fftSize;

    minFreqHz = juce::jmax(1.0f, minFreqHz);
    maxFreqHz = juce::jmax(minFreqHz + 1.0f, maxFreqHz);

    const float logMin = std::log10(minFreqHz);
    const float logMax = std::log10(maxFreqHz);

    for (int band = 0; band < numBands; ++band)
    {
        const float t0 = (float)band / (float)numBands;
        const float t1 = (float)(band + 1) / (float)numBands;

        const float freqLo = std::pow(10.0f, logMin + t0 * (logMax - logMin));
        const float freqHi = std::pow(10.0f, logMin + t1 * (logMax - logMin));

        int binLo = juce::jlimit(0, numBins - 1, (int)(freqLo / binWidth));
        int binHi = juce::jlimit(binLo, numBins - 1, (int)(freqHi / binWidth));

        float peak = 0.0f;
        for (int bin = binLo; bin <= binHi; ++bin)
            peak = juce::jmax(peak, magnitudes[bin]);

        bands[(size_t)band] = std::sqrt(juce::jlimit(0.0f, 1.0f, peak)); //Normalizzazione FFT
    }

    return bands;
}