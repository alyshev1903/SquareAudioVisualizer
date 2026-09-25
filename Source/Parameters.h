#pragma once
#include <JuceHeader.h>
#include <fstream>

#define MIN_FREQ_AUDIBLE  20
#define MAX_FREQ_AUDIBLE  20000
#define NUM_AGENTS 6

#define DEFAULT_WINDOW_OPENED false
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
	static const String nameDimensionMode = "dimensionMode";
	static const String nameMode3D = "mode3D";
	static const String nameCubeSubdivision = "cubeSubdiv";
	static const String nameCubeSize = "cubeSize";
	static const String nameCubePointSize = "cubePointSize";
	static const String nameCubeDisplacement = "cubeDisplacement";
	static const String nameWaterfallSubdivision = "waterfallSubdiv";
	static const String nameWaterfallSize = "waterfallSize";
	static const String nameWaterfallPointSize = "waterfallPointSize";
	static const String nameWaterfallDisplacement = "waterfallDisplacement";
	static const String nameColorSeed = "colorSeed";
	static const String nameCubeRotationSpeed = "cubeRotationSpeed";


	static const int defaultSubdivisions = 25;
	static const int defaultSize = 100;
	static const int defaultPointSize = 3;
	static const int defaultPointOrTexture = 0;
	static const int defaultDimensionMode = 0;
	static const int defaultMode3D = 0;
	static const int defaultCubeSubdivisions = 10;
	static const int defaultWaterfallSubdivisions = 20;
	static const float defaultTranslationX = 0.0f;
	static const float defaultTranslationY = 0.0f;
	static const float defaultTranslationZ = 0.0f;
	static const float defaultSensitivity = 0.1f;
	static const float defaultInputLevelDb = 0.0f;
	static const bool defaultZcrEnabled = false;
	static const bool defaultPitchEnabled = false;
	static const int defaultCubeSize = 100;
	static const int defaultCubePointSize = 3;
	static const int defaultCubeDisplacement = 30;
	static const int defaultWaterfallSize = 100;
	static const int defaultWaterfallPointSize = 3;
	static const int defaultWaterfallDisplacement = 40;
	static const float defaultColorSeed = 0.0f;
	static const float defaultCubeRotationSpeed = 10.0f;

	static constexpr int envelopeAgentIndex = 0;
	static constexpr int zcrAgentIndex = 1;
	static constexpr int pitchAgentIndex = 2;
	static constexpr int cubeBreatheAgentIndex = 3; //Envelope 
	static constexpr int cubePointSizeAgentIndex = 4; //ZCR
	static constexpr int cubeRotationAgentIndex = 5; //Pitch

	static const StringArray PointOrTextureOptions = { "Point", "Texture" };
	static const StringArray DimensionModeOptions = { "2D", "3D" };
	static const StringArray Mode3DOptions = { "Spectrogram", "Visualizer" };

	static AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
	{
		std::vector<std::unique_ptr<RangedAudioParameter>> params;

		int id = 1;

		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameSubdivision, id++), "Subdivision",2, 100, defaultSubdivisions));
		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameSize, id++), "Size",10, 500, defaultSize));
		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(namePointSize, id++), "Point Size",1, 8, defaultPointSize));
		params.push_back(std::make_unique<AudioParameterChoice>(ParameterID(namePointOrTexture, id++), "Point or Texture",PointOrTextureOptions, defaultPointOrTexture));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameInputLevel, id++), "Input Level", NormalisableRange<float>(-36.0f, 12.0f, 0.1f), defaultInputLevelDb));
		params.push_back(std::make_unique<AudioParameterBool>(ParameterID(nameZcrEnabled, id++), "ZCR Effect", defaultZcrEnabled));
		params.push_back(std::make_unique<AudioParameterBool>(ParameterID(namePitchEnabled, id++), "Pitch Effect", defaultPitchEnabled));
		params.push_back(std::make_unique<AudioParameterChoice>(ParameterID(nameDimensionMode, id++), "Dimension Mode", DimensionModeOptions, defaultDimensionMode));
		params.push_back(std::make_unique<AudioParameterChoice>(ParameterID(nameMode3D, id++), "3D Mode", Mode3DOptions, defaultMode3D));
		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameCubeSubdivision, id++), "Cube Subdivisions", 2, 24, defaultCubeSubdivisions));
		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameCubeSize, id++), "Cube Size", 10, 500, defaultCubeSize));
		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameCubePointSize, id++), "Cube Point Size", 1, 8, defaultCubePointSize));
		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameCubeDisplacement, id++), "Cube Displacement", 0, 150, defaultCubeDisplacement));
		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameWaterfallSubdivision, id++), "Waterfall Subdivisions", 2, 60, defaultWaterfallSubdivisions));
		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameWaterfallSize, id++), "Waterfall Size", 10, 500, defaultWaterfallSize));
		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameWaterfallPointSize, id++), "Waterfall Point Size", 1, 8, defaultWaterfallPointSize));
		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameWaterfallDisplacement, id++), "Waterfall Displacement", 0, 150, defaultWaterfallDisplacement));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameColorSeed, id++), "Color Seed", NormalisableRange<float>(0.0f, 100.0f, 1.0f), defaultColorSeed));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameCubeRotationSpeed, id++), "Cube Rotation Speed", NormalisableRange<float>(0.0f, 100.0f, 1.0f), defaultCubeRotationSpeed));

		return { params.begin(), params.end() };
	}
}