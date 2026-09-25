#include "WaterfallRenderer.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "FFTProcessor.h"
#include "ColorUtils.h"
#include "BinaryData.h"

#include "vendor/glm/gtc/matrix_transform.hpp"

using namespace ::juce::gl;

WaterfallRenderer::WaterfallRenderer(SharedAudioData& sharedData)
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

    glm::vec3 eye(0.0f, 150.0f, 400.0f);
    camera = std::make_unique<Camera>(eye, glm::normalize(-eye));

    rebuildGeometry();
}

WaterfallRenderer::~WaterfallRenderer() {}

void WaterfallRenderer::rebuildGeometry()
{
    const int n = subdivisions + 1;
    history.assign((size_t)n, std::vector<float>((size_t)n, 0.0f));

    vertices.assign((size_t)n * (size_t)n, GridVertex{});

    const float step = size / (float)subdivisions;
    for (int row = 0; row < n; ++row)
    {
        for (int col = 0; col < n; ++col)
        {
            GridVertex v{};
            v.x = -size / 2.0f + (float)col * step;
            v.y = 0.0f;
            v.z = -size / 2.0f + (float)row * step;
            v.r = v.g = v.b = 1.0f;
            v.pointSize = pointSize;
            vertices[(size_t)(row * n + col)] = v;
        }
    }

    vao = std::make_unique<VertexArray>();
    vertexBuffer = std::make_unique<VertexBuffer>(vertices.data(), (unsigned int)(vertices.size() * sizeof(GridVertex)), GL_DYNAMIC_DRAW);

    VertexBufferLayout layout;
    Square::buildLayout(layout);
    vao->AddBuffer(*vertexBuffer, layout);
}

void WaterfallRenderer::updateFromAudio()
{
    std::vector<float> spectrum;
    int binsAvailable = sharedAudioData.readSpectrum(spectrum);
    if (binsAvailable <= 0)
        return;

    double sampleRate = sharedAudioData.readSampleRate();
    const int n = subdivisions + 1;
    std::vector<float> currentBands = FFTProcessor::computeBands(spectrum.data(), sampleRate, n, minFreqHz, maxFreqHz);

    history.pop_back();
    history.insert(history.begin(), currentBands);

    for (int row = 0; row < n; ++row)
    {
        for (int col = 0; col < n; ++col)
        {
            float value = history[(size_t)row][(size_t)col];
            GridVertex& v = vertices[(size_t)(row * n + col)];
            v.y = value * displacementScale;
            ColorUtils::heatColor(value, v.r, v.g, v.b, colorSeed);
            v.pointSize = pointSize;
        }
    }

    vertexBuffer->SetData(vertices.data(), (unsigned int)(vertices.size() * sizeof(GridVertex)));
}

void WaterfallRenderer::render(int currentWidth, int currentHeight)
{
    updateFromAudio();

    GLCall(glClearColor(0.05f, 0.05f, 0.05f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GLCall(glViewport(0, 0, currentWidth, currentHeight));

    shader->Bind();
    camera->Matrix(45.0f, 0.1f, 2000.0f, *shader, currentWidth, currentHeight);
    shader->SetUniform1i("u_UseTexture", 0);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
    shader->SetUniformMat4f("u_Model", model);

    vao->Bind();
    GLCall(glDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size()));
}

void WaterfallRenderer::setSubdivisions(int newSubdivisions) { subdivisions = juce::jmax(1, newSubdivisions); rebuildGeometry();}
void WaterfallRenderer::setSize(float newSize) { size = newSize; rebuildGeometry(); }
void WaterfallRenderer::setPointSize(float newPointSize) { pointSize = newPointSize; }
void WaterfallRenderer::setDisplacementScale(float newScale) {displacementScale = newScale; }