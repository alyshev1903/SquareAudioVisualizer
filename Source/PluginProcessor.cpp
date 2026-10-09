
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

    auto& waterfallDisplacementAgent = squareVisual.getAgent(Parameters::waterfallDisplacementAgentIndex);
    waterfallDisplacementAgent.setFeature(FeatureType::ENVELOPE);
    waterfallDisplacementAgent.setAgentType(AgentType::AUDIO);
    waterfallDisplacementAgent.setSourceType(SourceType::FLUID);
    waterfallDisplacementAgent.setActive(true);

    auto& waterfallPointSizeAgent = squareVisual.getAgent(Parameters::waterfallPointSizeAgentIndex);
    waterfallPointSizeAgent.setFeature(FeatureType::ZCR);
    waterfallPointSizeAgent.setAgentType(AgentType::AUDIO);
    waterfallPointSizeAgent.setSourceType(SourceType::FLUID);
    waterfallPointSizeAgent.setActive(true);
    waterfallPointSizeAgent.setRemapValue(4.0f);
    waterfallPointSizeAgent.setSmoothingFactor(0.8f);

    auto& lfoAgent = squareVisual.getAgent(Parameters::lfoAgentIndex);
    lfoAgent.setAgentType(AgentType::OSC);
    lfoAgent.setSourceType(SourceType::FLUID);
    lfoAgent.setSmoothingFactor(1.0f);
    lfoAgent.setActive(true);

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
    monoBuffer.setSize(1, samplesPerBlock);
    bandFilter.prepare(sampleRate, samplesPerBlock);

    for (auto& agent : squareVisual.getAgents())
    {
        if (agent.isActive() && agent.getAgentType() == AgentType::AUDIO)
            agent.prepareToPlay(sampleRate);
    }
}

void SquareAudioVisualizerAudioProcessor::releaseResources()
{
    
}

void SquareAudioVisualizerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    if (numChannels == 0 || numSamples == 0)
        return;

    auto sr = getSampleRate();

    float inputLevelDb = *parameters.getRawParameterValue(Parameters::nameInputLevel);
    float inputGain = juce::Decibels::decibelsToGain(inputLevelDb);

    monoBuffer.setSize(1, numSamples, false, false, true);
    monoBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
    for (int ch = 1; ch < numChannels; ++ch)
        monoBuffer.addFrom(0, 0, buffer, ch, 0, numSamples);
    if (numChannels > 1)
        monoBuffer.applyGain(1.0f / (float)numChannels);

    bandFilter.setCutoffs(parameters.getRawParameterValue(Parameters::nameFilterLowCut)->load(),
        parameters.getRawParameterValue(Parameters::nameFilterHighCut)->load());
    bandFilter.process(monoBuffer.getWritePointer(0), numSamples);

    squareVisual.processAudioBlock(monoBuffer, sr, inputGain);

    const bool  lfoSync = parameters.getRawParameterValue(Parameters::nameLfoSync)->load() > 0.5f;
    const float lfoRate = parameters.getRawParameterValue(Parameters::nameLfoRate)->load();
    const int   lfoShape = (int)parameters.getRawParameterValue(Parameters::nameLfoShape)->load();
    const int   lfoDivision = juce::jlimit(0, Parameters::LfoDivisionOptions.size() - 1, (int)parameters.getRawParameterValue(Parameters::nameLfoDivision)->load());

    double bpm = 120.0;
    double ppq = 0.0;
    bool   hostPlaying = false;

    if (auto* playHead = getPlayHead())
    {
        if (const auto position = playHead->getPosition())
        {
            if (const auto hostBpm = position->getBpm())
                bpm = *hostBpm;

            if (const auto hostPpq = position->getPpqPosition())
            {
                ppq = *hostPpq;
                hostPlaying = position->getIsPlaying();
            }
        }
    }

    auto& lfoAgent = squareVisual.getAgent(Parameters::lfoAgentIndex);
    lfoAgent.setSourceType(lfoSync ? SourceType::SYNC : SourceType::FLUID);
    lfoAgent.setOscillator(lfoRate, lfoShape, Parameters::lfoDivisionBeats[lfoDivision], bpm, ppq, hostPlaying);

    const int   gateTarget = (int)parameters.getRawParameterValue(Parameters::nameGateTarget)->load();
    const float gateThreshold = parameters.getRawParameterValue(Parameters::nameGateThreshold)->load();

    for (int i = 1; i < Parameters::GateTargetOptions.size(); ++i)
    {
        auto& gatedAgent = squareVisual.getAgent(Parameters::gateTargetAgentIndex[i]);
        gatedAgent.setSourceType(i == gateTarget ? SourceType::ON_OFF : SourceType::FLUID);
        gatedAgent.setThreshold(gateThreshold);
    }

    for (auto& agent : squareVisual.getAgents())
        agent.processBlock(monoBuffer, sr);
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
