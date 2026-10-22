#pragma once

#include <JuceHeader.h>
#include <juce_opengl/juce_opengl.h>
#include <memory>
#include <vector>
#include <array>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "Camera.h"
#include "Cube.h"
#include "SharedAudioData.h"
#include "Parameters.h"
#include "vendor/glm/glm.hpp"

class CubeRenderer
{
public:
    CubeRenderer(SharedAudioData& sharedData);
    ~CubeRenderer();

    void render(int currentWidth, int currentHeight);

    void setSubdivisions(int newSubdivisions);
    void setSize(float newSize);
    void setPointSize(float newPointSize);
    void setDisplacementScale(float newScale);
    void setColorSeed(float seed) { colorSeed = seed;}
    void setRotationSpeed(float speed) { baseRotationSpeed = speed; }
    void setLfoTarget(int target) { lfoTarget = target; }
    void setBrightnessAmount(float amount) { brightnessAmount = amount; }
    void setEchoAmount(float amount) { echoAmount = amount; }
    void setEchoDelay(float delayMs) { echoDelayMs = delayMs; }

    Camera* getCamera() {return camera.get(); }

private:
    void rebuildGeometry();
    void updateFromAudio();

    SharedAudioData& sharedAudioData;

    std::unique_ptr<Cube> cube;
    std::unique_ptr<VertexArray> vao;
    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Camera> camera;

    std::vector<GridVertex> vertices;

    int   subdivisions = Parameters::defaultCubeSubdivisions;
    float size = (float)Parameters::defaultSize;
    float pointSize = (float)Parameters::defaultPointSize;
    float displacementScale = 30.0f;
    float minFreqHz = Parameters::minFreqAudible;
    float maxFreqHz = Parameters::maxFreqAudible;
    float colorSeed = 0.0f;
	float baseRotationSpeed = 0.01f;
    int lfoTarget = Parameters::lfoTargetOff;
    float brightnessAmount = 0.0f;
	float pale = 0.0f;

	float echoAmount = 0.0f;
	float echoDelayMs = 100.0f;
	float lastFrameTimeMs = 0.0f;

    float     rotationAngle = 0.0f;
    glm::vec3 translation = glm::vec3(0.0f);

    struct TrailSample
    {
        double timeMs = 0.0;
        float  angle = 0.0f;
        float  scale = 1.0f;
    };

    static constexpr int trailHistorySize = 256;
    static constexpr int trailCopies = 12;
    std::array<TrailSample, trailHistorySize> trailHistory;
    int trailHead = 0;
    int trailCount = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CubeRenderer)
};