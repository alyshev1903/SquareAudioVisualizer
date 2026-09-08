#pragma once

#include "JuceHeader.h"
#include <juce_opengl/juce_opengl.h>
#include "vendor/glm/glm.hpp" 

#include <memory>
#include <vector>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Square.h"
#include "SquareVisual.h"
#include "SharedAudioData.h"
#include "Parameters.h"

class SquareGridRenderer
{
public:
    SquareGridRenderer(SquareVisual& visual, SharedAudioData& sharedData);
    ~SquareGridRenderer();

    void render(int currentWidth, int currentHeight);

    void setSubdivisions(int newSubdivisions);
    void setSize(float newSize);

    void setPointSize(float newPointSize);
    void setDisplacementScale(float newScale);
    void setFrequencyRange(float newMinHz, float newMaxHz);

    bool loadTexture(const juce::File& imageFile);
    void setUseTexture(bool shouldUseTexture);

    void setTranslation(const glm::vec3& newTranslation) { translation = newTranslation; }
    glm::vec3 getTranslation() const { return translation; }

    Camera* getCamera() { return camera.get(); }

    void setZcrEffectEnabled(bool enabled) { zcrEffectEnabled = enabled;}
    void setPitchEffectEnabled(bool enabled) { pitchEffectEnabled = enabled; }

private:
    void rebuildGrid();
    void updateFromAudio();
    void highlightPitchColumn();

    SquareVisual& visual;
    SharedAudioData& sharedAudioData;

    std::unique_ptr<Square>       square;
    std::unique_ptr<VertexArray>  vao;
    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<Shader>       shader;
    std::unique_ptr<Texture>      texture;
    std::unique_ptr<Camera>       camera;

    std::vector<GridVertex> vertices;

    int   subdivisions = Parameters::defaultSubdivisions;
    float size = (float)Parameters::defaultSize;
    float pointSize = (float)Parameters::defaultPointSize;
    float displacementScale = 40.0f;
    float minFreqHz = Parameters::minFreqAudible;
    float maxFreqHz = Parameters::maxFreqAudible;
    bool  useTexture = false;
    bool zcrEffectEnabled = false;
    bool pitchEffectEnabled = false;
    glm::vec3 translation = glm::vec3(0.0f);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SquareGridRenderer)
};