#include "Brightness.h"

Brightness::Brightness(FeatureType type)
    : FeaturesExtractor(type)
{}

Brightness::~Brightness()
{}

void Brightness::processSample(float sample)
{}

void Brightness::setFFT(float* newData)
{
    fftData = newData;
}

void Brightness::setFFTSize(int newFFTsize)
{
    fftSize = newFFTsize;
}

void Brightness::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
}

void Brightness::computeValueFromFFTData()
{
    if (fftData == nullptr)
        return;

    const int   numBins = fftSize / 2;
    const float binWidth = (float)(sampleRate / (double)fftSize);

    float weightedSum = 0.0f;
    float totalMagnitude = 0.0f;

    for (int bin = 1; bin < numBins; ++bin)
    {
        const float magnitude = fftData[bin];
        weightedSum += magnitude * (float)bin * binWidth;
        totalMagnitude += magnitude;
    }

    fftData = nullptr;

    if (totalMagnitude < silenceThreshold)
        return;

    const float centroidHz = weightedSum / totalMagnitude;
    const float target = juce::jlimit(0.0f, 1.0f,
        std::log(centroidHz / lowCentroidHz) / std::log(highCentroidHz / lowCentroidHz));

    if (!hasValue)
    {
        brightness = target;
        hasValue = true;
    }
    else
    {
        brightness += smoothing * (target - brightness);
    }
}

float Brightness::getValue()
{
    return brightness;
}

String Brightness::getName()
{
    return "Brightness";
}