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

    const double nowMs = juce::Time::getMillisecondCounterHiRes();
    const float dtSeconds = lastFrameTimeMs > 0.0
        ? (float)juce::jlimit(0.0, 100.0, nowMs - lastFrameTimeMs) * 0.001f
        : 0.0f;
    lastFrameTimeMs = nowMs;

    float zcrValue = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(Parameters::modulationSource(lfoTarget, Parameters::lfoTargetCubePointSize, Parameters::cubePointSizeAgentIndex)));
    float currentPointSize = pointSize * (1.0f + zcrValue * 1.5f);

    float lfoValue = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(Parameters::lfoAgentIndex));
    float brightnessValue = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(Parameters::brightnessAgentIndex));
    float hue = colorSeed + (lfoTarget == Parameters::lfoTargetColorShift ? lfoValue : 0.0f);
    pale = brightnessAmount * brightnessValue;

    vertices = cube->buildVertexData(bands, displacementScale, currentPointSize, hue);
    vertexBuffer->SetData(vertices.data(), (unsigned int)(vertices.size() * sizeof(GridVertex)));

    float pitchValue = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(Parameters::modulationSource(lfoTarget, Parameters::lfoTargetCubeRotation, Parameters::cubeRotationAgentIndex)));
    rotationAngle += (baseRotationSpeed + pitchValue * 0.05f) * dtSeconds * Parameters::animationReferenceFps;
    rotationAngle = std::fmod(rotationAngle, 2.0f * juce::MathConstants<float>::pi);
}

void CubeRenderer::render(int currentWidth, int currentHeight)
{
    updateFromAudio();

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthMask(GL_TRUE));
    GLCall(glClearColor(0.05f, 0.05f, 0.05f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GLCall(glViewport(0, 0, currentWidth, currentHeight));

    shader->Bind();
    shader->SetUniform1f("u_Pale", pale);
    camera->Matrix(45.0f, 0.1f, 2000.0f, *shader, currentWidth, currentHeight);
    shader->SetUniform1i("u_UseTexture", 0);

    float envelopeValue = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(Parameters::modulationSource(lfoTarget, Parameters::lfoTargetCubeBreathe, Parameters::cubeBreatheAgentIndex)));
    float scale = 1.0f + envelopeValue * 0.3f;

    const double nowMs = juce::Time::getMillisecondCounterHiRes();
    if (trailCount > 0 && nowMs - trailHistory[(size_t)trailHead].timeMs > 200.0)
        trailCount = 0;

    trailHead = (trailHead + 1) % trailHistorySize;
    trailHistory[(size_t)trailHead] = { nowMs, rotationAngle, scale };
    trailCount = juce::jmin(trailCount + 1, trailHistorySize);

    if (echoAmount > 0.001f)
    {
        GLCall(glDepthMask(GL_FALSE));

        for (int k = trailCopies; k >= 1; --k)
        {
            const double targetAgeMs = echoDelayMs * (double)k / (double)trailCopies;

            TrailSample sample = trailHistory[(size_t)trailHead];
            for (int j = 0; j < trailCount; ++j)
            {
                sample = trailHistory[(size_t)((trailHead - j + trailHistorySize) % trailHistorySize)];
                if (nowMs - sample.timeMs >= targetAgeMs)
                    break;
            }

            const float fade = 1.0f - (float)(k - 1) / (float)trailCopies;

            glm::mat4 trailModel = glm::translate(glm::mat4(1.0f), translation);
            trailModel = glm::rotate(trailModel, sample.angle, glm::vec3(0.3f, 1.0f, 0.2f));
            trailModel = glm::scale(trailModel, glm::vec3(sample.scale));
            shader->SetUniformMat4f("u_Model", trailModel);

            DrawPoints(*shader, *vao, (int)vertices.size(), false, echoAmount * 0.4f * fade, 0.55f + 0.35f * fade);
        }

        GLCall(glDepthMask(GL_TRUE));
    }

    glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
    model = glm::rotate(model, rotationAngle, glm::vec3(0.3f, 1.0f, 0.2f));
    model = glm::scale(model, glm::vec3(scale));
    shader->SetUniformMat4f("u_Model", model);

    DrawPoints(*shader, *vao, (int)vertices.size(), true);
}

void CubeRenderer::setSubdivisions(int newSubdivisions) { subdivisions = juce::jmax(1, newSubdivisions); rebuildGeometry(); }
void CubeRenderer::setSize(float newSize) {size = newSize; rebuildGeometry(); }
void CubeRenderer::setPointSize(float newPointSize) { pointSize = newPointSize; }
void CubeRenderer::setDisplacementScale(float newScale) { displacementScale = newScale; }