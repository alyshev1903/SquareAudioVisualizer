#pragma once
#include <JuceHeader.h>
#include <fstream>

#define MIN_FREQ_AUDIBLE  20
#define MAX_FREQ_AUDIBLE  20000
#define MAX_NUM_VISUALS 10

#define DEFAULT_WINDOW_OPENED false
#define DEFAULT_FPS 60
#define DEFAULT_SHADER "Basic"

#define DEFAULT_AGENT_ACTIVE true
#define DEFAULT_FEATURE_TYPE FeatureType::ENVELOPE
#define DEFAULT_SOURCE_TYPE  SourceType::FLUID
#define DEFAULT_AGENT_TYPE   AgentType::FIXED

#define DEFAULT_NUM_CHANNEL 1
#define DEFAULT_REMAP_VALUE 1.0f
#define DEFAULT_THRESHOLD_VALUE 0.5f
#define	DEFAULT_SMOOTHING_VALUE 0.5f

#define DEFAULT_CHANNEL_CHOICE 0
namespace Parameters
{
	static constexpr auto fftOrder = 12;
	static constexpr auto fftSize = 1 << fftOrder;
	static constexpr auto minFreqAudible = MIN_FREQ_AUDIBLE;
	static constexpr auto maxFreqAudible = MAX_FREQ_AUDIBLE;

	static const String nameSubdivision = "subdiv";
	static const String nameSize = "size";
	static const String namePointSize = "pointSize";
	static const String namePointOrTexture = "pointOrTexture";
	static const String TranslationX = "translX";
	static const String TranslationY = "translY";
	static const String TranslationZ = "translZ";
	static const String Sensitivity = "sens";
	static const String nameInputLevel = "inputLevel";
	static const String nameZcrEnabled = "zcrEnabled";
	static const String namePitchEnabled = "pitchEnabled";


	static const int defaultSubdivisions = 25;
	static const int defaultSize = 100;
	static const int defaultPointSize = 3;
	static const int defaultPointOrTexture = 0;
	static const float defaultTranslationX = 0.0f;
	static const float defaultTranslationY = 0.0f;
	static const float defaultTranslationZ = 0.0f;
	static const float defaultSensitivity = 0.1f;
	static const float defaultInputLevelDb = 0.0f;
	static const bool defaultZcrEnabled = false;
	static const bool defaultPitchEnabled = false;

	static constexpr int envelopeAgentIndex = 0;
	static constexpr int zcrAgentIndex = 1;
	static constexpr int pitchAgentIndex = 2;

	static const StringArray PointOrTextureOptions = { "Point", "Texture" };

	static AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
	{
		std::vector<std::unique_ptr<RangedAudioParameter>> params;

		int id = 1;

		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameSubdivision, id++), "Subdivision",2, 200, defaultSubdivisions));
		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameSize, id++), "Size",10, 500, defaultSize));
		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(namePointSize, id++), "Point Size",1, 20, defaultPointSize));
		params.push_back(std::make_unique<AudioParameterChoice>(ParameterID(namePointOrTexture, id++), "Point or Texture",PointOrTextureOptions, defaultPointOrTexture));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(TranslationX, id++), "Translation X", NormalisableRange<float>(-480.0f, 480.0f), defaultTranslationX));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(TranslationY, id++), "Translation Y", NormalisableRange<float>(-480.0f, 480.0f), defaultTranslationY));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(TranslationZ, id++), "Translation Z", NormalisableRange<float>(-480.0f, 480.0f), defaultTranslationZ));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(Sensitivity, id++), "Sensitivity", NormalisableRange<float>(0.001f, 1.0f), defaultSensitivity));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameInputLevel, id++), "Input Level", NormalisableRange<float>(-36.0f, 12.0f), defaultInputLevelDb));
		params.push_back(std::make_unique<AudioParameterBool>(ParameterID(nameZcrEnabled, id++), "ZCR Effect", defaultZcrEnabled));
		params.push_back(std::make_unique<AudioParameterBool>(ParameterID(namePitchEnabled, id++), "Pitch Effect", defaultPitchEnabled));

		return { params.begin(), params.end() };
	}

	static void addListenerToAllParameters(AudioProcessorValueTreeState& valueTreeState, AudioProcessorValueTreeState::Listener* listener)
	{
		std::unique_ptr<XmlElement> xml(valueTreeState.copyState().createXml());

		for (auto* element : xml->getChildWithTagNameIterator("PARAM"))
		{
			const String& id = element->getStringAttribute("id");
			valueTreeState.addParameterListener(id, listener);
		}
	}
}