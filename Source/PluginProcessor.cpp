
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SquareAudioVisualizerAudioProcessor::SquareAudioVisualizerAudioProcessor()
    : parameters(*this, nullptr, "PARAMS", Parameters::createParameterLayout())
{
    Parameters::addListenerToAllParameters(parameters, this);

    for (auto& visual : squareVisuals)
        visual.init(sharedAudioData);

    auto& envelopeAgent = squareVisuals[0].getAgent(Parameters::envelopeAgentIndex);
    envelopeAgent.setFeature(FeatureType::ENVELOPE);
    envelopeAgent.setAgentType(AgentType::AUDIO);
    envelopeAgent.setSourceType(SourceType::FLUID);
    envelopeAgent.setActive(true);

    auto& zcrAgent = squareVisuals[0].getAgent(Parameters::zcrAgentIndex);
    zcrAgent.setFeature(FeatureType::ZCR);
    zcrAgent.setAgentType(AgentType::AUDIO);
    zcrAgent.setSourceType(SourceType::FLUID);
    zcrAgent.setActive(true);

    auto& pitchAgent = squareVisuals[0].getAgent(Parameters::pitchAgentIndex);
    pitchAgent.setFeature(FeatureType::PITCH);
    pitchAgent.setAgentType(AgentType::AUDIO);
    pitchAgent.setSourceType(SourceType::FLUID);
    pitchAgent.setActive(true);
}


SquareAudioVisualizerAudioProcessor::~SquareAudioVisualizerAudioProcessor()
{
}

//==============================================================================
const juce::String SquareAudioVisualizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool   SquareAudioVisualizerAudioProcessor::acceptsMidi() const { return false; }

bool   SquareAudioVisualizerAudioProcessor::producesMidi() const { return false; }

double SquareAudioVisualizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SquareAudioVisualizerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SquareAudioVisualizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SquareAudioVisualizerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SquareAudioVisualizerAudioProcessor::getProgramName (int index)
{
    return {};
}

void SquareAudioVisualizerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SquareAudioVisualizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	for (auto& visual : squareVisuals)
	{
		for (auto& agent : visual.getAgents())
		{
            if (agent.isActive() && agent.getAgentType() == AgentType::AUDIO) {
                agent.prepareToPlay(sampleRate);
            }
			
		}
	}
}

void SquareAudioVisualizerAudioProcessor::releaseResources()
{
    
}

void SquareAudioVisualizerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto sr = getSampleRate();

    float inputLevelDb = *parameters.getRawParameterValue(Parameters::nameInputLevel);
    float inputGain = juce::Decibels::decibelsToGain(inputLevelDb);


    squareVisuals[0].processAudioBlock(buffer, sr, inputGain);

    for (auto& visual : squareVisuals)
    {
        for (auto& agent : visual.getAgents())
            agent.processBlock(buffer, sr);
    }
}

void SquareAudioVisualizerAudioProcessor::parameterChanged(const String& paramID, float newValue)
{
	// Handle parameter changes here
}

bool SquareAudioVisualizerAudioProcessor::getWindowOpened()
{
	return windowOpened;
}

void SquareAudioVisualizerAudioProcessor::setWindowOpened(bool value)
{
	windowOpened = value;
}

//==============================================================================
bool SquareAudioVisualizerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SquareAudioVisualizerAudioProcessor::createEditor()
{
    return new WrappedPluginEditor (*this, parameters, squareVisuals);
}

//==============================================================================
void SquareAudioVisualizerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SquareAudioVisualizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(ValueTree::fromXml(*xmlState));
}

void SquareAudioVisualizerAudioProcessor::debugVisualParams()
{
	for (auto& visual : squareVisuals)
	{
		DBG("Visual Name: " << visual.getName());
		for (auto& agent : visual.getAgents())
		{
			DBG("Agent Name: " << agent.getParamName());
			DBG("Feature Type: " << static_cast<int>(agent.getFeature()));
			DBG("Source Type: " << static_cast<int>(agent.getSourceType()));
			DBG("Agent Type: " << static_cast<int>(agent.getAgentType()));
			DBG("Remap Value: " << agent.getRemapValue());
			DBG("Smoothing Factor: " << agent.getSmoothingFactor());
			DBG("Threshold: " << agent.getThreshold());
			/*DBG("Sidechain: " << agent.isSidechain());
			DBG("Active: " << agent.isActive());*/
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SquareAudioVisualizerAudioProcessor();
}
