#include "SquareGridRenderer.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "FFTProcessor.h"

#include "BinaryData.h"

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

using namespace ::juce::gl;

SquareGridRenderer::SquareGridRenderer(SquareVisual& visualIn, SharedAudioData& sharedDataIn)
    : visual(visualIn), sharedAudioData(sharedDataIn)
{
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_PROGRAM_POINT_SIZE));

    shader = std::make_unique<Shader>((const void*)BinaryData::Basic_shader, BinaryData::Basic_shaderSize);
    shader->Bind();
    shader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
    shader->SetUniform1i("u_Texture", 0);
    shader->SetUniform1i("u_UseTexture", 0);

    camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 500.0f));

    rebuildGrid();
}

SquareGridRenderer::~SquareGridRenderer() {}

void SquareGridRenderer::rebuildGrid()
{
    square = std::make_unique<Square>(subdivisions, size);
    vertices = square->generatePointGrid();

    vao = std::make_unique<VertexArray>();
    vertexBuffer = std::make_unique<VertexBuffer>(vertices.data(), (unsigned int)(vertices.size() * sizeof(GridVertex)), GL_DYNAMIC_DRAW);

    VertexBufferLayout layout;
    Square::buildLayout(layout);
    vao->AddBuffer(*vertexBuffer, layout);
}

void SquareGridRenderer::updateFromAudio()
{
    std::vector<float> spectrum;
    int binsAvailable = sharedAudioData.readSpectrum(spectrum);
    if (binsAvailable <= 0)
        return;

    double sampleRate = sharedAudioData.readSampleRate();
    int numBands = subdivisions + 1;

    std::vector<float> bands = FFTProcessor::computeBands(
        spectrum.data(), sampleRate, numBands, minFreqHz, maxFreqHz);

    float envelopeValue = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(Parameters::envelopeAgentIndex));
    float effectiveDisplacement = displacementScale * (0.3f + 0.7f * envelopeValue);

    float currentPointSize = pointSize;
    if (zcrEffectEnabled)
    {
        float zcrValue = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(Parameters::zcrAgentIndex));
        currentPointSize = pointSize * (1.0f + zcrValue * 1.5f);
    }

    square->applyColumnSpectrum(vertices, bands, effectiveDisplacement, currentPointSize);

    if (pitchEffectEnabled)
        highlightPitchColumn();

    vertexBuffer->SetData(vertices.data(), (unsigned int)(vertices.size() * sizeof(GridVertex)));
}

void SquareGridRenderer::highlightPitchColumn()
{
    float pitchHz = sharedAudioData.readPitch();
    if (pitchHz < minFreqHz || pitchHz > maxFreqHz)
        return;

    float logMin = std::log10(juce::jmax(1.0f, minFreqHz));
    float logMax = std::log10(juce::jmax(minFreqHz + 1.0f, maxFreqHz));
    float t = (std::log10(pitchHz) - logMin) / (logMax - logMin);

    int cols = subdivisions + 1;
    int highlightCol = juce::jlimit(0, cols - 1, (int)std::round(t * (float)(cols - 1)));

    for (int i = 0; i < cols; ++i)
    {
        GridVertex& vert = vertices[(size_t)(i * cols + highlightCol)];
        vert.r = vert.g = vert.b = 1.0f;
        vert.pointSize *= 1.8f;
    }
}

void SquareGridRenderer::render(int currentWidth, int currentHeight)
{
    updateFromAudio();

    GLCall(glClearColor(0.05f, 0.05f, 0.05f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
    GLCall(glViewport(0, 0, currentWidth, currentHeight));

    shader->Bind();
    camera->Matrix(45.0f, 0.1f, 1000.0f, *shader, currentWidth, currentHeight);
    shader->SetUniform1i("u_UseTexture", useTexture ? 1 : 0);

    if (useTexture && texture != nullptr)
        texture->Bind(0);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
    shader->SetUniformMat4f("u_Model", model);

    vao->Bind();
    GLCall(glDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size()));
}

void SquareGridRenderer::setSubdivisions(int newSubdivisions)
{
    subdivisions = juce::jmax(1, newSubdivisions);
    rebuildGrid();
}

void SquareGridRenderer::setSize(float newSize)
{
    size = newSize;
    rebuildGrid();
}

void SquareGridRenderer::setPointSize(float newPointSize)
{
    pointSize = newPointSize;
    for (auto& v : vertices)
        v.pointSize = pointSize;
    vertexBuffer->SetData(vertices.data(), (unsigned int)(vertices.size() * sizeof(GridVertex)));
}

void SquareGridRenderer::setDisplacementScale(float newScale)
{
    displacementScale = newScale;
}

void SquareGridRenderer::setFrequencyRange(float newMinHz, float newMaxHz)
{
    minFreqHz = newMinHz;
    maxFreqHz = newMaxHz;
}

bool SquareGridRenderer::loadTexture(const juce::File& imageFile)
{
    if (!imageFile.existsAsFile())
        return false;

    auto newTexture = std::make_unique<Texture>(imageFile.getFullPathName().toStdString());
    if (newTexture->GetWidth() <= 0 || newTexture->GetHeight() <= 0)
        return false;

    texture = std::move(newTexture);
    useTexture = true;
    return true;
}

void SquareGridRenderer::setUseTexture(bool shouldUseTexture)
{
    useTexture = shouldUseTexture && texture != nullptr;
}