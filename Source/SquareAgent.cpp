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
    featuresExtractor->setFFTSize(spectrumAnalyzer.getFFTSize());
    featuresExtractor->setSampleRate(sampleRate);
}

void SquareAgent::processBlock(juce::AudioBuffer<float> buffer, double sampleRate)
{
    if (agentType == AgentType::AUDIO && isActive())
    {
        int numSamples = buffer.getNumSamples();
        if (buffer.getNumChannels() > 0)
        {
            auto* channelData = buffer.getReadPointer(0);

            for (int s = 0; s < numSamples; ++s)
            {
                featuresExtractor->processSample(channelData[s]);
                if (featuresExtractor->isFreqDomain())
                    spectrumAnalyzer.pushIntoFifo(channelData[s]);
            }
            if (featuresExtractor->isFreqDomain())
            {
                if (spectrumAnalyzer.getNextFFTBlockReady())
                {
                    spectrumAnalyzer.computeFFT();
                    featuresExtractor->setFFT(spectrumAnalyzer.getFFTData());
                    featuresExtractor->computeValueFromFFTData();
                    spectrumAnalyzer.resetFFTBlock();
                }
            }
        }
    }
    computeValue();

    if (sharedAudioData != nullptr)
        sharedAudioData->writeAgentValue(agentIndex, value.get());
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
            if (featuresExtractor->getType() == FeatureType::PITCH)
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
                if (finalValue > remapValue.get() * threshold.get())
                {
                    value.set(remapValue.get());
                }
                else
                {
                    value.set(0.0f);
                }
                break;

            case SourceType::SYNC:
                break;

            default:
                break;
            }
            break;
        case AgentType::OSC:
            break;

        default:
            break;
        }


    }
}
void SquareAgent::setSampleRate(double newSampleRate)
{
	featuresExtractor->setSampleRate(newSampleRate);
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
    featureType = newValue;
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
void SquareAgent::setBpm(double newValue)
{
    bpm = newValue;
}
void SquareAgent::setPpqPeriod(double newValue)
{
    ppqPeriod = newValue;
}
void SquareAgent::setSmoothingFactor(float newValue)
{
    smoothingFactor = newValue;
}
void SquareAgent::setThreshold(float newValue)
{
    threshold.set(newValue);
}

void SquareAgent::setParamName(String newName)
{
    paramName = newName;
}
void SquareAgent::setSideChain(bool newValue)
{
    sidechainSource = newValue;
}

void SquareAgent::setActive(bool newValue)
{
    active = newValue;
}

FeatureType SquareAgent::getFeature()
{
    return featureType;
}
SourceType SquareAgent::getSourceType()
{
    return sourceType;
}

AgentType SquareAgent::getAgentType()
{
    return agentType;
}

float SquareAgent::getValue()
{
    return value.get();
}
float SquareAgent::getFeatureValue()
{
    return featureValue.get();
}

float SquareAgent::getRemapValue()
{
    return remapValue.get();
}
float SquareAgent::getSmoothingFactor()
{
    return smoothingFactor.get();
}
float SquareAgent::getMinFreq()
{
    return minFreq.get();
}
float SquareAgent::getMaxFreq()
{
    return maxFreq.get();
}
float SquareAgent::getThreshold()
{
    return threshold.get();
}

String SquareAgent::getParamName()
{
    return paramName;
}
bool  SquareAgent::isSidechain()
{
    return sidechainSource;
}
bool  SquareAgent::isActive()
{
    return active;
}