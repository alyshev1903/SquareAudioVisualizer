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

	void processData();

private:
	static constexpr int maxBufferSize = 256;
	std::array<float, maxBufferSize> audioBuffer{};
	int bufferIndex = 0;
	float zcr = 0.0f;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZCR)
};