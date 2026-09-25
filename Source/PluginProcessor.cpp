
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SquareAudioVisualizerAudioProcessor::SquareAudioVisualizerAudioProcessor()
    : parameters(*this, nullptr, "PARAMS", Parameters::createParameterLayout())
{
    squareVisual.init(sharedAudioData);

    auto& envelopeAgent = squareVisual.getAgent(Parameters::envelopeAgentIndex);
    envelopeAgent.setFeature(FeatureType::ENVELOPE);
    envelopeAgent.setAgentType(AgentType::AUDIO);
    envelopeAgent.setSourceType(SourceType::FLUID);
    envelopeAgent.setActive(true);

    auto& zcrAgent = squareVisual.getAgent(Parameters::zcrAgentIndex);
    zcrAgent.setFeature(FeatureType::ZCR);
    zcrAgent.setAgentType(AgentType::AUDIO);
    zcrAgent.setSourceType(SourceType::FLUID);
    zcrAgent.setActive(true);
    zcrAgent.setRemapValue(4.0f);
    zcrAgent.setSmoothingFactor(0.8f);

    auto& pitchAgent = squareVisual.getAgent(Parameters::pitchAgentIndex);
    pitchAgent.setFeature(FeatureType::PITCH);
    pitchAgent.setAgentType(AgentType::AUDIO);
    pitchAgent.setSourceType(SourceType::FLUID);
    pitchAgent.setActive(true);

    auto& breatheAgent = squareVisual.getAgent(Parameters::cubeBreatheAgentIndex);
    breatheAgent.setFeature(FeatureType::ENVELOPE);
    breatheAgent.setAgentType(AgentType::AUDIO);
    breatheAgent.setSourceType(SourceType::FLUID);
    breatheAgent.setActive(true);

    auto& cubePointSizeAgent = squareVisual.getAgent(Parameters::cubePointSizeAgentIndex);
    cubePointSizeAgent.setFeature(FeatureType::ZCR);
    cubePointSizeAgent.setAgentType(AgentType::AUDIO);
    cubePointSizeAgent.setSourceType(SourceType::FLUID);
    cubePointSizeAgent.setActive(true);

    auto& cubeRotationAgent = squareVisual.getAgent(Parameters::cubeRotationAgentIndex);
    cubeRotationAgent.setFeature(FeatureType::PITCH);
    cubeRotationAgent.setAgentType(AgentType::AUDIO);
    cubeRotationAgent.setSourceType(SourceType::FLUID);
    cubeRotationAgent.setActive(true);
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
    for (auto& agent : squareVisual.getAgents())
    {
        if (agent.isActive() && agent.getAgentType() == AgentType::AUDIO)
            agent.prepareToPlay(sampleRate);
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

    squareVisual.processAudioBlock(buffer, sr, inputGain);

    for (auto& agent : squareVisual.getAgents())
        agent.processBlock(buffer, sr);
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
    return new WrappedPluginEditor(*this, parameters, squareVisual);
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

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SquareAudioVisualizerAudioProcessor();
}
