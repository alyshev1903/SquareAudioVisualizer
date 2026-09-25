#pragma once
#include "SquareAgent.h"
#include "Parameters.h"
#include "FFTProcessor.h"
#include "SharedAudioData.h"

typedef std::array<SquareAgent, NUM_AGENTS>    SquareAgents;

class SquareVisual
{
public:
    SquareVisual();
    ~SquareVisual();

    void init(SharedAudioData& sharedData);
    void processAudioBlock(const juce::AudioBuffer<float>& buffer, double sampleRate, float inputGain);

    void setAlpha(float newAlpha);
    void setName(String newName);

    float getAlpha();
    String getName();

    SquareAgent& getAgent(int index);
    SquareAgents& getAgents();

private:

    SquareAgents agents;
    float alpha = 1.0f;
    String name;

    FFTProcessor spectrumAnalyzer;
    SharedAudioData* sharedAudioData = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SquareVisual)
};