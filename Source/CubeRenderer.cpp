#include "CubeRenderer.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "FFTProcessor.h"
#include "BinaryData.h"

#include <cmath>

#include "vendor/glm/gtc/matrix_transform.hpp"

using namespace ::juce::gl;

CubeRenderer::CubeRenderer(SharedAudioData& sharedData)
    : sharedAudioData(sharedData)
{
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_PROGRAM_POINT_SIZE));
    GLCall(glEnable(GL_DEPTH_TEST));

    shader = std::make_unique<Shader>((const void*)BinaryData::Basic_shader, BinaryData::Basic_shaderSize);
    shader->Bind();
    shader->SetUniform1i("u_Texture", 0);
    shader->SetUniform1i("u_UseTexture", 0);

    camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 500.0f));

    rebuildGeometry();
}

CubeRenderer::~CubeRenderer() {}

void CubeRenderer::rebuildGeometry()
{
    cube = std::make_unique<Cube>(subdivisions, size);
    vertices = cube->buildVertexData({}, displacementScale, pointSize, colorSeed);

    vao = std::make_unique<VertexArray>();
    vertexBuffer = std::make_unique<VertexBuffer>(vertices.data(), (unsigned int)(vertices.size() * sizeof(GridVertex)), GL_DYNAMIC_DRAW);

    VertexBufferLayout layout;
    Square::buildLayout(layout);
    vao->AddBuffer(*vertexBuffer, layout);
}

void CubeRenderer::updateFromAudio()
{
    std::vector<float> spectrum;
    int binsAvailable = sharedAudioData.readSpectrum(spectrum);
    if (binsAvailable <= 0)
        return;

    double sampleRate = sharedAudioData.readSampleRate();
    int numBands = subdivisions + 1;

    std::vector<float> bands = FFTProcessor::computeBands(spectrum.data(), sampleRate, numBands, minFreqHz, maxFreqHz);

    float zcrValue = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(Parameters::cubePointSizeAgentIndex));
    float currentPointSize = pointSize * (1.0f + zcrValue * 1.5f);

    vertices = cube->buildVertexData(bands, displacementScale, currentPointSize, colorSeed);
    vertexBuffer->SetData(vertices.data(), (unsigned int)(vertices.size() * sizeof(GridVertex)));

    float pitchValue = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(Parameters::cubeRotationAgentIndex));
    rotationAngle += (baseRotationSpeed + pitchValue * 0.05f);
    rotationAngle = std::fmod(rotationAngle, 2.0f * juce::MathConstants<float>::pi);
}

void CubeRenderer::render(int currentWidth, int currentHeight)
{
    updateFromAudio();

    GLCall(glClearColor(0.05f, 0.05f, 0.05f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GLCall(glViewport(0, 0, currentWidth, currentHeight));

    shader->Bind();
    camera->Matrix(45.0f, 0.1f, 2000.0f, *shader, currentWidth, currentHeight);
    shader->SetUniform1i("u_UseTexture", 0);

    float envelopeValue = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(Parameters::cubeBreatheAgentIndex));
    float scale = 1.0f + envelopeValue * 0.3f;

    glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
    model = glm::rotate(model, rotationAngle, glm::vec3(0.3f, 1.0f, 0.2f));
    model = glm::scale(model, glm::vec3(scale));
    shader->SetUniformMat4f("u_Model", model);

    vao->Bind();
    GLCall(glDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size()));
}

void CubeRenderer::setSubdivisions(int newSubdivisions) { subdivisions = juce::jmax(1, newSubdivisions); rebuildGeometry(); }
void CubeRenderer::setSize(float newSize) {size = newSize; rebuildGeometry(); }
void CubeRenderer::setPointSize(float newPointSize) { pointSize = newPointSize; }
void CubeRenderer::setDisplacementScale(float newScale) { displacementScale = newScale; }