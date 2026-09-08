#include "SquareVisual.h"

SquareVisual::SquareVisual()
{
    startTimer(DEFAULT_FPS);
}

SquareVisual::~SquareVisual()
{
    for (auto& agent : agents)
    {
        agent.deleteFeature();
    }
    stopTimer();
}

void SquareVisual::init(SharedAudioData& sharedData)
{
    sharedAudioData = &sharedData;
    for (int i = 0; i < (int)agents.size(); ++i)
        agents[(size_t)i].prepare(sharedData, i);
}

void SquareVisual::processAudioBlock(const juce::AudioBuffer<float>& buffer, double sampleRate, float inputGain)
{
    if (sharedAudioData == nullptr || buffer.getNumChannels() == 0)
        return;

    sharedAudioData->writeSampleRate(sampleRate);

    auto* channelData = buffer.getReadPointer(0);
    for (int s = 0; s < buffer.getNumSamples(); ++s)
        spectrumAnalyzer.pushIntoFifo(channelData[s] * inputGain);

    if (spectrumAnalyzer.getNextFFTBlockReady())
    {
        spectrumAnalyzer.computeFFT();
        sharedAudioData->writeSpectrum(spectrumAnalyzer.getFFTData(), FFTProcessor::fftSize / 2);
        spectrumAnalyzer.resetFFTBlock();
    }
}

void SquareVisual::setAlpha(float newAlpha) { alpha = newAlpha; }
void SquareVisual::setName(String newName) { name = newName; }

SquareAgent& SquareVisual::getAgent(int index)
{
    jassert(index < MAX_NUM_VISUALS);
    return agents[index];
}

SquareAgents& SquareVisual::getAgents() { return agents; }
float SquareVisual::getAlpha() { return alpha; }
String SquareVisual::getName() { return name; }
unsigned int SquareVisual::getShaderID() { return shaderID; }

void SquareVisual::timerCallback()
{
    for (auto& agent : agents)
        agent.computeValue();
}