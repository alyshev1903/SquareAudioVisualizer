#pragma once
#include "FeaturesExtractor.h"
#include "../Source/Parameters.h"

class Pitch : public FeaturesExtractor
{
public:
    Pitch(FeatureType t);
    ~Pitch();

    void processSample(float sample) override;
    void setFFT(float* newData) override;
    void setFFTSize(int newFFTsize) override;
    void setSampleRate(double newSampleRate) override;
    void computeValueFromFFTData() override;
    float getValue() override;
    String getName() override;

    float getDominantFrequency() const;

    const std::vector<float>& getHarmonics() const;

private:
   
    float binToHz(int bin) const;

    int findDominantBin() const;

    void findHarmonics(int fundamentalBin);

    std::vector<float> fftData;
    std::vector<float> harmonics;   

    double sampleRate = 44100.0;
    int    fftSize = 4096;
    float  dominantFreq = 0.0f;

    float minFreq = MIN_FREQ_AUDIBLE;
    float maxFreq = MAX_FREQ_AUDIBLE;

    float peakThreshold = 0.01f;

    int maxHarmonics = 6;

    bool fftDataReady = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Pitch)
};