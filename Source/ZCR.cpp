#include "ZCR.h"

ZCR::ZCR(FeatureType type)
	: FeaturesExtractor(type)
{
	
}

ZCR::~ZCR()
{

}

void ZCR::processSample(float sample)
{
	audioBuffer.add(sample);
	if (audioBuffer.size() > maxBufferSize)
	{
		processData(audioBuffer);
		audioBuffer.clear();
	}
}

void ZCR::processData(juce::Array<float> audioBuffer)
{
	int zeroCrossings = 0;
	for (int i = 1; i < audioBuffer.size(); ++i)
	{
		if ((audioBuffer[i] >= 0.0f && audioBuffer[i-1] < 0.0f) || (audioBuffer[i - 1] < 0.0f && audioBuffer[i] >= 0.0f))
		{
			++zeroCrossings;
		}
	}
	zcr = static_cast<float>(zeroCrossings) / static_cast<float>(audioBuffer.size() - 1);
}

float ZCR::getValue()
{
	return zcr;
}

void ZCR::setFFT(float* newData)
{
	
}

void ZCR::setFFTSize(int newFFTsize)
{

}

void ZCR::setSampleRate(double newSampleRate)
{

}

void ZCR::computeValueFromFFTData()
{

}

String ZCR::getName()
{
	return "ZCR";
}