#include "Pitch.h"

Pitch::Pitch(FeatureType type)
    : FeaturesExtractor(type)
{}

Pitch::~Pitch() {}

void Pitch::processSample(float sample)
{
    
}

void Pitch::setFFT(float* newData)
{
    if (fftSize > 0)
    {
        fftData.assign(newData, newData + fftSize);
        fftDataReady = true;
    }
}

void Pitch::setFFTSize(int newFFTsize)
{
    fftSize = newFFTsize;
    fftData.resize(fftSize, 0.0f);
}

void Pitch::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
}

void Pitch::computeValueFromFFTData()
{
    if (!fftDataReady || fftData.empty())
        return;

    int dominantBin = findDominantBin();

    if (dominantBin > 0)
    {
        dominantFreq = binToHz(dominantBin);
        findHarmonics(dominantBin);
    }

    fftDataReady = false;
}

float Pitch::getValue()
{
    if (dominantFreq <= 0.0f)
        return 0.0f;

    float logMin = std::log(minFreq);
    float logMax = std::log(maxFreq);
    float logFreq = std::log(dominantFreq);

    return juce::jmap(logFreq, logMin, logMax, 0.0f, 1.0f);
}

float Pitch::getDominantFrequency() const
{
    return dominantFreq;
}

const std::vector<float>& Pitch::getHarmonics() const
{
    return harmonics;
}

String Pitch::getName()
{
    return "Pitch";
}

float Pitch::binToHz(int bin) const
{
    return (float)bin * (float)sampleRate / (float)fftSize;
}

int Pitch::findDominantBin() const
{
    if (fftData.empty())
        return 0;
    
    int minBin = (int)(minFreq * fftSize / sampleRate);
    int maxBin = (int)(maxFreq * fftSize / sampleRate);
    maxBin = juce::jmin(maxBin, (int)fftData.size() - 1);

    // trova il bin con magnitudine massima
    float maxMag = 0.0f;
    int   maxBinIdx = minBin;

    for (int i = minBin; i <= maxBin; ++i)
    {
        if (fftData[i] > maxMag)
        {
            maxMag = fftData[i];
            maxBinIdx = i;
        }
    }
 
    if (maxMag < peakThreshold)
        return 0;

    return maxBinIdx;
}

void Pitch::findHarmonics(int fundamentalBin)
{
    harmonics.clear();

    if (fundamentalBin <= 0)
        return;

    for (int h = 2; h <= maxHarmonics + 1; ++h)
    {
        int harmonicBin = fundamentalBin * h;

        if (harmonicBin >= (int)fftData.size())
            break;

        int searchRange = juce::jmax(1, fundamentalBin / 4);
        int startBin = juce::jmax(0, harmonicBin - searchRange);
        int endBin = juce::jmin((int)fftData.size() - 1, harmonicBin + searchRange);

        float localMax = 0.0f;
        int   localMaxBin = harmonicBin;

        for (int i = startBin; i <= endBin; ++i)
        {
            if (fftData[i] > localMax)
            {
                localMax = fftData[i];
                localMaxBin = i;
            }
        }

        if (localMax > peakThreshold)
            harmonics.push_back(binToHz(localMaxBin));
    }
}