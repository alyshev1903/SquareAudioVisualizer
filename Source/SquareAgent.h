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
    void processBlock(juce::AudioBuffer<float> buffer, double sampleRate);
    void computeValue();
    float getValue();

    void setSampleRate(double sampleRate);
    void setFeature(FeatureType value);
    void setSourceType(SourceType  newValue);
    void setAgentType(AgentType   newValue);
    void setRemapValue(float newValue);
    void setSmoothingFactor(float newValue);
    void setThreshold(float newValue);
    void setBpm(double newValue);
    void setPpqPeriod(double newValue);
    void setSideChain(bool newValue);
    void setActive(bool newValue);
    void setParamName(String newName);

    FeatureType getFeature();
    SourceType  getSourceType();
    AgentType   getAgentType();
    float getRemapValue();
    float getSmoothingFactor();
    float getMinFreq();
    float getMaxFreq();
    float getFeatureValue();
    float getThreshold();
    bool  isSidechain();
    bool  isActive();
    String getParamName();
private:
    String paramName;
    FeatureType   featureType = DEFAULT_FEATURE_TYPE;
    SourceType    sourceType = DEFAULT_SOURCE_TYPE;
    AgentType     agentType = DEFAULT_AGENT_TYPE;

    Atomic<float> remapValue = DEFAULT_REMAP_VALUE;
    Atomic<float> threshold = DEFAULT_THRESHOLD_VALUE;
    Atomic<float> smoothingFactor = DEFAULT_SMOOTHING_VALUE;
    Atomic<float> minFreq = MIN_FREQ_AUDIBLE;
    Atomic<float> maxFreq = MAX_FREQ_AUDIBLE;
    Atomic<float> value = 0.0f;
    Atomic<float> featureValue = 0.0f;

    bool  sidechainSource = false;
    int   numChannel = DEFAULT_NUM_CHANNEL;
    bool  active = DEFAULT_AGENT_ACTIVE;

    int   agentIndex = -1;

    double bpm = 0.0;
    double ppqPeriod = 0.0;
    double syncPhaseIncrement = 0.0;

    std::unique_ptr<FeaturesExtractor> featuresExtractor;

    SharedAudioData* sharedAudioData = nullptr;

    FFTProcessor  spectrumAnalyzer;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SquareAgent)
};