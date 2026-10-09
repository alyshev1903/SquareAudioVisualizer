#pragma once
#include <JuceHeader.h>
#include <fstream>

#define MIN_FREQ_AUDIBLE  20
#define MAX_FREQ_AUDIBLE  20000
#define NUM_AGENTS 9

#define DEFAULT_WINDOW_OPENED false

#define DEFAULT_AGENT_ACTIVE true
#define DEFAULT_SOURCE_TYPE  SourceType::FLUID
#define DEFAULT_AGENT_TYPE   AgentType::FIXED

#define DEFAULT_REMAP_VALUE 1.0f
#define DEFAULT_THRESHOLD_VALUE 0.5f
#define	DEFAULT_SMOOTHING_VALUE 0.5f
namespace Parameters
{
	static constexpr auto minFreqAudible = MIN_FREQ_AUDIBLE;
	static constexpr auto maxFreqAudible = MAX_FREQ_AUDIBLE;

	static const String nameSubdivision = "subdiv";
	static const String nameSize = "size";
	static const String namePointSize = "pointSize";
	static const String namePointOrTexture = "pointOrTexture";
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

	static const String nameLfoTarget = "lfoTarget";
	static const String nameLfoRate = "lfoRate";
	static const String nameLfoShape = "lfoShape";
	static const String nameLfoSync = "lfoSync";
	static const String nameLfoDivision = "lfoDivision";

	static const String nameGateTarget = "gateTarget";
	static const String nameGateThreshold = "gateThreshold";

	static const String nameFilterLowCut = "filterLowCut";
	static const String nameFilterHighCut = "filterHighCut";

	static const int defaultSubdivisions = 25;
	static const int defaultSize = 100;
	static const int defaultPointSize = 3;
	static const int defaultPointOrTexture = 0;
	static const int defaultDimensionMode = 0;
	static const int defaultMode3D = 0;
	static const int defaultCubeSubdivisions = 10;
	static const int defaultWaterfallSubdivisions = 20;
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

	static const int   defaultLfoTarget = 0;
	static const float defaultLfoRate = 1.0f;
	static const int   defaultLfoShape = 0;
	static const bool  defaultLfoSync = false;
	static const int   defaultLfoDivision = 4;

	static const int   defaultGateTarget = 0;
	static const float defaultGateThreshold = 0.5f;

	static const float defaultFilterLowCut = 20.0f;
	static const float defaultFilterHighCut = 20000.0f;

	static constexpr int envelopeAgentIndex = 0;
	static constexpr int zcrAgentIndex = 1;
	static constexpr int pitchAgentIndex = 2;
	static constexpr int cubeBreatheAgentIndex = 3; //Envelope 
	static constexpr int cubePointSizeAgentIndex = 4; //ZCR
	static constexpr int cubeRotationAgentIndex = 5; //Pitch
	static constexpr int lfoAgentIndex = 6;
	static constexpr int waterfallDisplacementAgentIndex = 7;
	static constexpr int waterfallPointSizeAgentIndex = 8;

	static constexpr int lfoTargetOff = 0;
	static constexpr int lfoTargetColorShift = 1;
	static constexpr int lfoTarget2DPointSize = 2;
	static constexpr int lfoTargetCubeBreathe = 3;
	static constexpr int lfoTargetCubePointSize = 4;
	static constexpr int lfoTargetCubeRotation = 5;
	static constexpr int lfoTargetWaterfallDisplacement = 6;
	static constexpr int lfoTargetWaterfallPointSize = 7;

	static constexpr float lfoDivisionBeats[7] = { 16.0f, 8.0f, 4.0f, 2.0f, 1.0f, 0.5f, 0.25f };
	static constexpr int gateTargetAgentIndex[7] = { -1, zcrAgentIndex, cubeBreatheAgentIndex, cubePointSizeAgentIndex,cubeRotationAgentIndex, waterfallDisplacementAgentIndex, waterfallPointSizeAgentIndex };

	static constexpr int modulationSource(int lfoTarget, int effectTarget, int audioAgentIndex)
	{
		return lfoTarget == effectTarget ? lfoAgentIndex : audioAgentIndex;
	}

	static const StringArray PointOrTextureOptions = { "Point", "Texture" };

	static const StringArray DimensionModeOptions = { "2D", "3D" };
	static const StringArray Mode3DOptions = { "Spectrogram", "Visualizer" };

	static const StringArray LfoTargetOptions = { "Off", "Color shift", "2D Point size", "Cube Breathe", "Cube Point size", "Cube Rotation", "Waterfall Displacement", "Waterfall Point size" };
	static const StringArray LfoShapeOptions = { "Sine", "Triangle", "Saw", "Square" };
	static const StringArray LfoDivisionOptions = { "4 bar", "2 bar", "1 bar", "1/2", "1/4", "1/8", "1/16" };

	static const StringArray GateTargetOptions = { "Off", "2D Point size", "Cube Breathe", "Cube Point size", "Cube Rotation", "Waterfall Displacement", "Waterfall Point size" };

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

		params.push_back(std::make_unique<AudioParameterChoice>(ParameterID(nameLfoTarget, id++), "LFO Target", LfoTargetOptions, defaultLfoTarget));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameLfoRate, id++), "LFO Rate", NormalisableRange<float>(0.05f, 10.0f, 0.01f, 0.4f), defaultLfoRate));
		params.push_back(std::make_unique<AudioParameterChoice>(ParameterID(nameLfoShape, id++), "LFO Shape", LfoShapeOptions, defaultLfoShape));
		params.push_back(std::make_unique<AudioParameterBool>(ParameterID(nameLfoSync, id++), "LFO Sync", defaultLfoSync));
		params.push_back(std::make_unique<AudioParameterChoice>(ParameterID(nameLfoDivision, id++), "LFO Division", LfoDivisionOptions, defaultLfoDivision));

		params.push_back(std::make_unique<AudioParameterChoice>(ParameterID(nameGateTarget, id++), "Gate Target", GateTargetOptions, defaultGateTarget));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameGateThreshold, id++), "Gate Threshold", NormalisableRange<float>(0.0f, 1.0f, 0.01f), defaultGateThreshold));

		NormalisableRange<float> filterRange((float)minFreqAudible, (float)maxFreqAudible, 1.0f);
		filterRange.setSkewForCentre(1000.0f);

		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameFilterLowCut, id++), "Filter Low Cut", filterRange, defaultFilterLowCut));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameFilterHighCut, id++), "Filter High Cut", filterRange, defaultFilterHighCut));

		return { params.begin(), params.end() };
	}
}