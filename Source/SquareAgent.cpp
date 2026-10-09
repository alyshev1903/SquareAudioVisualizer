#include "SquareAgent.h"

SquareAgent::SquareAgent() 
{

}

SquareAgent::~SquareAgent()
{
    deleteFeature();
}

void SquareAgent::prepare(SharedAudioData& sharedData, int index)
{
    sharedAudioData = &sharedData;
    agentIndex = index;
}

void SquareAgent::prepareToPlay(double sampleRate)
{
    featuresExtractor->setFFTSize(FFTProcessor::fftSize);
    featuresExtractor->setSampleRate(sampleRate);
}

void SquareAgent::processBlock(juce::AudioBuffer<float>& buffer, double sampleRate)
{
    if (agentType == AgentType::AUDIO && isActive() && buffer.getNumChannels() > 0)
    {
        auto* channelData = buffer.getReadPointer(0);
        for (int s = 0; s < buffer.getNumSamples(); ++s)
            featuresExtractor->processSample(channelData[s]);
    }
    else if (agentType == AgentType::OSC && isActive())
    {
        advanceOscillator(buffer.getNumSamples(), sampleRate);
    }

    computeValue();

    if (sharedAudioData != nullptr)
        sharedAudioData->writeAgentValue(agentIndex, value.get());
}

void SquareAgent::processSpectrum(float* fftData)
{
    if (agentType == AgentType::AUDIO && isActive() && featuresExtractor->isFreqDomain())
    {
        featuresExtractor->setFFT(fftData);
        featuresExtractor->computeValueFromFFTData();
    }
}

void SquareAgent::setOscillator(float rateHz, int shape, float beatsPerCycle, double bpm, double ppqPosition, bool lockToHost)
{
    oscRateHz = rateHz;
    oscShape = shape;
    oscBeatsPerCycle = beatsPerCycle;
    hostBpm = bpm;
    hostPpq = ppqPosition;
    hostLocked = lockToHost;
}

void SquareAgent::advanceOscillator(int numSamples, double sampleRate)
{
    if (sampleRate <= 0.0)
        return;

    const double blockSeconds = (double)numSamples / sampleRate;

    if (sourceType == SourceType::SYNC)
    {
        if (hostLocked)
            oscPhase = hostPpq / (double)oscBeatsPerCycle;
        else
            oscPhase += (hostBpm / 60.0) * blockSeconds / (double)oscBeatsPerCycle;
    }
    else
    {
        oscPhase += (double)oscRateHz * blockSeconds;
    }

    oscPhase -= std::floor(oscPhase);
    oscOutput = oscillatorShape(oscPhase);
}

float SquareAgent::oscillatorShape(double phase) const
{
    const float p = (float)phase;
    switch (oscShape)
    {
    case 0:  return 0.5f - 0.5f * std::cos(juce::MathConstants<float>::twoPi * p);
    case 1:  return 1.0f - std::abs(2.0f * p - 1.0f);
    case 2:  return p;
    default: return p < 0.5f ? 1.0f : 0.0f;
    }
}

void SquareAgent::computeValue()
{
    if (isActive())
    {
        float remappedValue;
        float finalValue;
        switch (agentType)
        {
        case AgentType::FIXED:
            remappedValue = remapValue.get();
            finalValue = smoothingFactor.get() * remappedValue + (1.0f - smoothingFactor.get()) * value.get();
            value.set(finalValue);
            break;
        case AgentType::AUDIO:
            featureValue.set(featuresExtractor->getValue());
            if (featuresExtractor->getType() == FeatureType::PITCH && agentIndex == Parameters::pitchAgentIndex)
            {
                auto* pitch = dynamic_cast<Pitch*>(featuresExtractor.get());
                if (pitch != nullptr)
                {
                    float fundamental = pitch->getDominantFrequency();
                    const auto& harmonics = pitch->getHarmonics();

                    sharedAudioData->writePitch(fundamental);
                    sharedAudioData->writeHarmonics(harmonics);
                }
            }

            remappedValue = jmap(featureValue.get(), 0.0f, 1.0f, 0.0f, remapValue.get());
            finalValue = smoothingFactor.get() * remappedValue + (1.0f - smoothingFactor.get()) * value.get();
            switch (sourceType)
            {
            case SourceType::FLUID:
                value.set(finalValue);
                break;
            case SourceType::ON_OFF:
            {
                const float onLevel = remapValue.get() * threshold.get();
                const float offLevel = onLevel * 0.8f;
                const bool  wasOn = value.get() > 0.0f;
                const bool  isOn = wasOn ? (remappedValue > offLevel) : (remappedValue > onLevel);

                value.set(isOn ? remapValue.get() : 0.0f);
                break;
            }

            case SourceType::SYNC:
                value.set(finalValue);
                break;

            default:
                break;
            }
            break;
        case AgentType::OSC:
            remappedValue = oscOutput * remapValue.get();
            finalValue = smoothingFactor.get() * remappedValue + (1.0f - smoothingFactor.get()) * value.get();
            value.set(finalValue);
            break;

        default:
            break;
        }


    }
}

void SquareAgent::setFeatureExtractor(FeatureType type)
{
    featuresExtractor.reset();
    featuresExtractor = createFeature(type);
}
void SquareAgent::deleteFeature()
{
    featuresExtractor.reset();
}

void SquareAgent::setFeature(FeatureType newValue)
{
    setFeatureExtractor(newValue);
}
void SquareAgent::setSourceType(SourceType newValue)
{
    sourceType = newValue;
}
void SquareAgent::setAgentType(AgentType newValue)
{
    agentType = newValue;
}

void SquareAgent::setRemapValue(float newValue)
{
    remapValue.set(newValue);
}
void SquareAgent::setSmoothingFactor(float newValue)
{
    smoothingFactor = newValue;
}
void SquareAgent::setThreshold(float newValue)
{
    threshold.set(newValue);
}

void SquareAgent::setActive(bool newValue)
{
    active = newValue;
}

AgentType SquareAgent::getAgentType()
{
    return agentType;
}

float SquareAgent::getValue()
{
    return value.get();
}

bool  SquareAgent::isActive()
{
    return active;
}