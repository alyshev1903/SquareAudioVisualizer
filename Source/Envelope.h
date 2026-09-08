#pragma once

#include "FeaturesExtractor.h"
#include "../Source/Parameters.h"

class Envelope : public FeaturesExtractor
{
public:
	Envelope(FeatureType t);
	~Envelope();

	void processSample(float sample) override;
	void setFFT(float* newData) override;
	void setFFTSize(int newFFTsize) override;
	void setSampleRate(double newSampleRate) override;
	void computeValueFromFFTData() override;
	float getValue() override;
	String getName() override;

private:

	juce::Array<float> audioBuffer;
	float smoothingFactor = 0.999f;
	float envelope;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Envelope)
};