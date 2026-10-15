#include "ZCR.h"

ZCR::ZCR(FeatureType type)
	: FeaturesExtractor(type)
{
	updateCoefficients();
}

ZCR::~ZCR()
{}

void ZCR::updateCoefficients()
{
	const auto smoothingCoef = [this](double ms)
	{
		return (float)(1.0 - std::exp(-1.0 / (0.001 * ms * sampleRate)));
	};

	fastCoef = smoothingCoef(fastTimeMs);
	slowCoef = smoothingCoef(slowTimeMs);
	releaseCoef = (float)std::exp(-1.0 / (0.001 * (double)releaseTimeMs * sampleRate));
}

void ZCR::setSampleRate(double newSampleRate)
{
	sampleRate = newSampleRate;
	updateCoefficients();
}

void ZCR::processSample(float sample)
{
	const float rectified = std::abs(sample);

	fastEnv += fastCoef * (rectified - fastEnv);
	slowEnv += slowCoef * (rectified - slowEnv);

	const float ratio = fastEnv / (slowEnv + noiseFloor);
	const float strength = juce::jlimit(0.0f, 1.0f, (ratio - ratioStart) / (ratioFull - ratioStart));

	pulse = juce::jmax(strength, pulse * releaseCoef);
}

float ZCR::getValue()
{
	return pulse;
}

void ZCR::setFFT(float* newData)
{}

void ZCR::setFFTSize(int newFFTsize)
{}

void ZCR::computeValueFromFFTData()
{}

String ZCR::getName()
{
	return "Onset";
}