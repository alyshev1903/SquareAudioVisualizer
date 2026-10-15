#pragma once

#include "FeaturesExtractor.h"
#include "../Source/Parameters.h"

class Brightness : public FeaturesExtractor
{
public:
    Brightness(FeatureType t);
    ~Brightness();

    void processSample(float sample) override;
    void setFFT(float* newData) override;
    void setFFTSize(int newFFTsize) override;
    void setSampleRate(double newSampleRate) override;
    void computeValueFromFFTData() override;
    float getValue() override;
    String getName() override;

private:
    const float* fftData = nullptr;
    int    fftSize = 2048;
    double sampleRate = 44100.0;
    float  brightness = 0.0f;
    bool   hasValue = false;

    static constexpr float lowCentroidHz = 200.0f;
    static constexpr float highCentroidHz = 8000.0f;
    static constexpr float silenceThreshold = 0.1f;
    static constexpr float smoothing = 0.3f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Brightness)
};