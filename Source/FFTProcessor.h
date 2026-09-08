#pragma once

#include <JuceHeader.h>
#include <vector>

#include "Parameters.h"

class FFTProcessor
{
public:
	FFTProcessor();
	~FFTProcessor();

	static constexpr auto fftOrder = 11;
	static constexpr auto fftSize = 1 << fftOrder;

	int getFFTSize();
	void computeFFT();
	void resetFFTBlock();
	float* getFFTData();
	bool getNextFFTBlockReady();

	void pushIntoFifo(float sample) noexcept;

	static std::vector<float> computeBands(const float* magnitudes,double sampleRate, int numBands, float minFreqHz, float maxFreqHz);

private:
	juce::dsp::FFT forwardFFT;
	juce::dsp::WindowingFunction<float> window;

	float fifo[fftSize];
	float fftData[fftSize * 2];
	int   fifoIndex = 0;
	bool  nextFFTBlockReady = false;

	int channelChoice = DEFAULT_CHANNEL_CHOICE;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTProcessor)
};