#pragma once

#include <JuceHeader.h>
#include <juce_opengl/juce_opengl.h>
#include <memory>
#include <vector>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "Camera.h"
#include "Square.h"
#include "SharedAudioData.h"
#include "Parameters.h"
#include "vendor/glm/glm.hpp"

class WaterfallRenderer
{
public:
    WaterfallRenderer(SharedAudioData& sharedData);
    ~WaterfallRenderer();

    void render(int currentWidth, int currentHeight);

    void setSubdivisions(int newSubdivisions);
    void setSize(float newSize);
    void setPointSize(float newPointSize);
    void setDisplacementScale(float newScale);
	void setColorSeed(float seed) { colorSeed = seed; }

    Camera* getCamera() { return camera.get(); }

private:
    void rebuildGeometry();
    void updateFromAudio();

    SharedAudioData& sharedAudioData;

    std::unique_ptr<VertexArray>  vao;
    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<Shader>       shader;
    std::unique_ptr<Camera>       camera;

    std::vector<GridVertex>         vertices;
    std::vector<std::vector<float>> history;

    int   subdivisions = Parameters::defaultWaterfallSubdivisions;
    float size = (float)Parameters::defaultSize;
    float pointSize = (float)Parameters::defaultPointSize;
    float displacementScale = 40.0f;
    float minFreqHz = Parameters::minFreqAudible;
    float maxFreqHz = Parameters::maxFreqAudible;
	float colorSeed = 0.0f;

    glm::vec3 translation = glm::vec3(0.0f);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaterfallRenderer)
};