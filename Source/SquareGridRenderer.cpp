#include "SquareGridRenderer.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "FFTProcessor.h"
#include "ColorUtils.h"

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

    float lfoValue = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(Parameters::lfoAgentIndex));
    float brightnessValue = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(Parameters::brightnessAgentIndex));
    float hue = colorSeed + (lfoTarget == Parameters::lfoTargetColorShift ? lfoValue : 0.0f) + brightnessAmount * brightnessValue;

    float currentPointSize = pointSize;
    if (zcrEffectEnabled || lfoTarget == Parameters::lfoTarget2DPointSize)
    {
        float pointSizeValue = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(
            Parameters::modulationSource(lfoTarget, Parameters::lfoTarget2DPointSize, Parameters::zcrAgentIndex)));
        currentPointSize = pointSize * (1.0f + pointSizeValue * 1.5f);
    }

    square->applyColumnSpectrum(vertices, bands, effectiveDisplacement, currentPointSize, hue);

    if (waveAmount > 0.001f)
    {
        updateWaves(hue, currentPointSize);
    }
    else
    {
        for (auto& w : waves)
            w.active = false;
    }

    if (pitchEffectEnabled)
        highlightPitchColumns(currentPointSize);

    vertexBuffer->SetData(vertices.data(), (unsigned int)(vertices.size() * sizeof(GridVertex)));
}

int SquareGridRenderer::columnForFrequency(float hz) const
{
    float logMin = std::log10(juce::jmax(1.0f, minFreqHz));
    float logMax = std::log10(juce::jmax(minFreqHz + 1.0f, maxFreqHz));
    float t = (std::log10(hz) - logMin) / (logMax - logMin);

    int cols = subdivisions + 1;
    return juce::jlimit(0, cols - 1, (int)std::round(t * (float)(cols - 1)));
}

void SquareGridRenderer::highlightColumn(int column, float brightness, float basePointSize, float sizeBoost)
{
    int cols = subdivisions + 1;
    for (int i = 0; i < cols; ++i)
    {
        GridVertex& vert = vertices[(size_t)(i * cols + column)];
        vert.r += (1.0f - vert.r) * brightness;
        vert.g += (1.0f - vert.g) * brightness;
        vert.b += (1.0f - vert.b) * brightness;
        vert.pointSize = juce::jmax(vert.pointSize, basePointSize * sizeBoost);
    }
}

void SquareGridRenderer::highlightPitchColumns(float basePointSize)
{
    float pitchHz = sharedAudioData.readPitch();
    if (pitchHz < minFreqHz || pitchHz > maxFreqHz)
        return;

    int numHarmonics = sharedAudioData.readHarmonicCount();
    for (int h = 0; h < numHarmonics; ++h)
    {
        float hz = sharedAudioData.readHarmonic(h);
        if (hz < minFreqHz || hz > maxFreqHz)
            continue;

        float fade = 1.0f - (float)h / (float)(numHarmonics + 1);
        highlightColumn(columnForFrequency(hz), 0.6f * fade, basePointSize, 1.0f + 0.4f * fade);
    }

    highlightColumn(columnForFrequency(pitchHz), 1.0f, basePointSize, 1.8f);
}

void SquareGridRenderer::render(int currentWidth, int currentHeight)
{
    updateFromAudio();
    
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthMask(GL_TRUE));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
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

    DrawPoints(*shader, *vao, (int)vertices.size(), !useTexture);
}

void SquareGridRenderer::spawnWave(float strength)
{
    const int cols = subdivisions + 1;

    float cx = random.nextFloat() * (float)(cols - 1);
    const float pitchHz = sharedAudioData.readPitch();
    if (pitchHz >= minFreqHz && pitchHz <= maxFreqHz)
        cx = (float)columnForFrequency(pitchHz);

    const float brightness = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(Parameters::brightnessAgentIndex));

    Wave* slot = &waves[0];
    for (auto& w : waves)
    {
        if (!w.active)
        {
            slot = &w;
            break;
        }
        if (w.age > slot->age)
            slot = &w;
    }

    slot->cx = cx;
    slot->cy = brightness * (float)(cols - 1);
    slot->age = 0.0f;
    slot->strength = strength;
    slot->tone = brightness;
    slot->active = true;
}

void SquareGridRenderer::updateWaves(float hue, float basePointSize)
{
    const double nowMs = juce::Time::getMillisecondCounterHiRes();
    const float dt = lastFrameTimeMs > 0.0
        ? (float)juce::jlimit(0.0, 100.0, nowMs - lastFrameTimeMs) * 0.001f
        : 0.0f;
    lastFrameTimeMs = nowMs;

    const float onset = juce::jlimit(0.0f, 1.0f, sharedAudioData.readAgentValue(Parameters::zcrAgentIndex));
    if (onset > 0.3f && onset - lastOnset > 0.1f && nowMs - lastWaveTimeMs > 100.0)
    {
        spawnWave(onset);
        lastWaveTimeMs = nowMs;
    }
    lastOnset = onset;

    const int   cols = subdivisions + 1;
    const float speed = 0.9f * (float)cols;
    const float thickness = juce::jmax(1.5f, 0.06f * (float)cols);
    const float lifetime = 1.3f;

    for (auto& w : waves)
    {
        if (!w.active)
            continue;

        w.age += dt;
        if (w.age > lifetime)
        {
            w.active = false;
            continue;
        }

        float r, g, b;
        ColorUtils::heatColor(0.4f + 0.6f * w.tone, r, g, b, hue);
        r += (1.0f - r) * 0.4f;
        g += (1.0f - g) * 0.4f;
        b += (1.0f - b) * 0.4f;

        const float radius = w.age * speed;
        const float amplitude = w.strength * (1.0f - w.age / lifetime) * waveAmount;
        const float cx = juce::jmin(w.cx, (float)(cols - 1));
        const float cy = juce::jmin(w.cy, (float)(cols - 1));

        for (int i = 0; i < cols; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                const float dx = (float)j - cx;
                const float dy = (float)i - cy;
                const float offset = std::sqrt(dx * dx + dy * dy) - radius;
                if (std::abs(offset) > 3.0f * thickness)
                    continue;

                const float q = offset / thickness;
                const float k = amplitude * std::exp(-q * q);
                if (k < 0.01f)
                    continue;

                GridVertex& v = vertices[(size_t)(i * cols + j)];
                v.r += (r - v.r) * k;
                v.g += (g - v.g) * k;
                v.b += (b - v.b) * k;
                v.pointSize = juce::jmax(v.pointSize, basePointSize * (1.0f + 1.6f * k));
            }
        }
    }
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
}

void SquareGridRenderer::setDisplacementScale(float newScale)
{
    displacementScale = newScale;
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