#pragma once

#include "JuceHeader.h"
#include "FeatureCreator.h"

#include "Envelope.h"
#include "ZCR.h"
#include "Pitch.h"

#include "FFTProcessor.h"

#include "SharedAudioData.h"

enum class SourceType { FLUID, ON_OFF, SYNC };
enum class AgentType { FIXED, AUDIO, OSC };

class SquareAgent : private FeatureCreator
{
public:
    SquareAgent();
    ~SquareAgent();

    void prepare(SharedAudioData& sharedData, int index);

    void setFeatureExtractor(FeatureType type);
    void deleteFeature();

    void prepareToPlay(double sampleRate);
    void processBlock(juce::AudioBuffer<float>& buffer, double sampleRate);
    void processSpectrum(float* fftData);
    void computeValue();
    float getValue();

    void setFeature(FeatureType value);
    void setSourceType(SourceType  newValue);
    void setAgentType(AgentType   newValue);
    void setRemapValue(float newValue);
    void setSmoothingFactor(float newValue);
    void setThreshold(float newValue);
	void setActive(bool newValue);

    void setOscillator(float rateHz, int shape, float beatsPerCycle, double bpm, double ppqPosition, bool lockToHost);

    AgentType   getAgentType();
    bool  isActive();
private:
    void  advanceOscillator(int numSamples, double sampleRate);
    float oscillatorShape(double phase) const;

    float  oscRateHz = 1.0f;
    int    oscShape = 0;
    float  oscBeatsPerCycle = 1.0f;
    double hostBpm = 120.0;
    double hostPpq = 0.0;
    bool   hostLocked = false;
    double oscPhase = 0.0;
    float  oscOutput = 0.0f;

    String paramName;
    SourceType    sourceType = DEFAULT_SOURCE_TYPE;
    AgentType     agentType = DEFAULT_AGENT_TYPE;

    Atomic<float> remapValue = DEFAULT_REMAP_VALUE;
    Atomic<float> threshold = DEFAULT_THRESHOLD_VALUE;
    Atomic<float> smoothingFactor = DEFAULT_SMOOTHING_VALUE;
    Atomic<float> value = 0.0f;
    Atomic<float> featureValue = 0.0f;

    bool  active = DEFAULT_AGENT_ACTIVE;

    int   agentIndex = -1;

    std::unique_ptr<FeaturesExtractor> featuresExtractor;

    SharedAudioData* sharedAudioData = nullptr;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SquareAgent)
};