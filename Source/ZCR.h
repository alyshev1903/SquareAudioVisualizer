#pragma once

#include "FeaturesExtractor.h"
#include "../Source/Parameters.h"

class ZCR : public FeaturesExtractor
{
public:
	ZCR(FeatureType t);
	~ZCR();
	void processSample(float sample) override;
	void setFFT(float* newData) override;
	void setFFTSize(int newFFTsize) override;
	void setSampleRate(double newSampleRate) override;
	void computeValueFromFFTData() override;
	float getValue() override;
	String getName() override;

	void processData(juce::Array<float> audioBuffer);

private:

	juce::Array<float> audioBuffer;
	int maxBufferSize = 256;
	float zcr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZCR)
};