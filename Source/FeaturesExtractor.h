#pragma once

#include "../Source/Parameters.h"

enum class FeatureType {ENVELOPE, ZCR, PITCH};

class FeaturesExtractor
{
public:

    FeaturesExtractor(FeatureType t)
		: type(t)
    {};
    virtual ~FeaturesExtractor() {};

    virtual void  processSample(float sample) = 0;
    virtual float getValue() = 0;
    virtual void  setFFT(float* newData) = 0;
    virtual void  computeValueFromFFTData() = 0;
    virtual void  setSampleRate(double sampleRate) = 0;
    virtual void  setFFTSize(int newFFTsize) = 0;
    virtual String getName() = 0;

    bool isFreqDomain()
    {
        return type == FeatureType::PITCH;
    }

    FeatureType getType()
    {
        return type;
    }

private:
protected:
    FeatureType type;
    int  numVisual;
    int  numAgent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FeaturesExtractor)
};