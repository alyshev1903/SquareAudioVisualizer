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

private:
	void updateCoefficients();

	double sampleRate = 44100.0;

	float fastEnv = 0.0f;
	float slowEnv = 0.0f;
	float pulse = 0.0f;

	float fastCoef = 0.0f;
	float slowCoef = 0.0f;
	float releaseCoef = 0.0f;

	static constexpr float fastTimeMs = 5.0f;
	static constexpr float slowTimeMs = 120.0f;
	static constexpr float releaseTimeMs = 120.0f;
	static constexpr float noiseFloor = 0.001f;
	static constexpr float ratioStart = 1.5f;
	static constexpr float ratioFull = 4.0f;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZCR)
};