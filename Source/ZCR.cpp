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
	audioBuffer[(size_t)bufferIndex] = sample;
	++bufferIndex;

	if (bufferIndex >= maxBufferSize)
	{
		processData();
		bufferIndex = 0;
	}
}

void ZCR::processData()
{
	int zeroCrossings = 0;
	for (int i = 1; i < maxBufferSize; ++i)
	{
		if ((audioBuffer[(size_t)i] >= 0.0f && audioBuffer[(size_t)i - 1] < 0.0f) ||
			(audioBuffer[(size_t)i - 1] < 0.0f && audioBuffer[(size_t)i] >= 0.0f))
		{
			++zeroCrossings;
		}
	}
	zcr = static_cast<float>(zeroCrossings) / static_cast<float>(maxBufferSize - 1);
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