#include "OpenGLComponent.h"
#include "vendor/glm/glm.hpp"
#include <cmath>

using namespace ::juce::gl;

OpenGLComponent::OpenGLComponent(SquareVisual& visualToRender, SharedAudioData& sharedData, AudioProcessorValueTreeState& stateToRead)
    : visual(visualToRender), sharedAudioData(sharedData), apvts(stateToRead)
{
    m_GLContext.setOpenGLVersionRequired(juce::OpenGLContext::openGL3_2);
    m_GLContext.setRenderer(this);
    m_GLContext.attachTo(*this);
    m_GLContext.setContinuousRepainting(true);
}

OpenGLComponent::~OpenGLComponent()
{
    m_GLContext.detach();
}

void OpenGLComponent::newOpenGLContextCreated()
{
    renderer2D = std::make_unique<SquareGridRenderer>(visual, sharedAudioData);
    cubeRenderer = std::make_unique<CubeRenderer>(sharedAudioData);
    waterfallRenderer = std::make_unique<WaterfallRenderer>(sharedAudioData);
}

void OpenGLComponent::pullParameters()
{
    if (renderer2D == nullptr)
        return;

    int   subdivisions = (int)apvts.getRawParameterValue(Parameters::nameSubdivision)->load();
    float size = apvts.getRawParameterValue(Parameters::nameSize)->load();
    float pointSize = apvts.getRawParameterValue(Parameters::namePointSize)->load();
    int   pointOrTexture = (int)apvts.getRawParameterValue(Parameters::namePointOrTexture)->load();
    bool  zcrEnabled = apvts.getRawParameterValue(Parameters::nameZcrEnabled)->load() > 0.5f;
    bool  pitchEnabled = apvts.getRawParameterValue(Parameters::namePitchEnabled)->load() > 0.5f;

    renderer2D->setZcrEffectEnabled(zcrEnabled);
    renderer2D->setPitchEffectEnabled(pitchEnabled);

    if (subdivisions != lastSubdivisions)
    {
        renderer2D->setSubdivisions(subdivisions);
        lastSubdivisions = subdivisions;
    }
    if (std::abs(size - lastSize) > 0.001f)
    {
        renderer2D->setSize(size);
        lastSize = size;
    }

    renderer2D->setPointSize(pointSize);
    renderer2D->setUseTexture(pointOrTexture == 1);

    if (cubeRenderer != nullptr)
    {
        int   cubeSubdivisions = (int)apvts.getRawParameterValue(Parameters::nameCubeSubdivision)->load();
        float cubeSize = apvts.getRawParameterValue(Parameters::nameCubeSize)->load();
        float cubePointSize = apvts.getRawParameterValue(Parameters::nameCubePointSize)->load();
        float cubeDisplacement = apvts.getRawParameterValue(Parameters::nameCubeDisplacement)->load();
        float cubeRotationSpeed = apvts.getRawParameterValue(Parameters::nameCubeRotationSpeed)->load() * 0.001f;

        if (cubeSubdivisions != lastCubeSubdivisions)
        {
            cubeRenderer->setSubdivisions(cubeSubdivisions);
            lastCubeSubdivisions = cubeSubdivisions;
        }
        if (std::abs(cubeSize - lastCubeSize) > 0.001f)
        {
            cubeRenderer->setSize(cubeSize);
            lastCubeSize = cubeSize;
        }
        cubeRenderer->setPointSize(cubePointSize);
        cubeRenderer->setDisplacementScale(cubeDisplacement);
        cubeRenderer->setRotationSpeed(cubeRotationSpeed);
    }

    if (waterfallRenderer != nullptr)
    {
        int   waterfallSubdivisions = (int)apvts.getRawParameterValue(Parameters::nameWaterfallSubdivision)->load();
        float waterfallSize = apvts.getRawParameterValue(Parameters::nameWaterfallSize)->load();
        float waterfallPointSize = apvts.getRawParameterValue(Parameters::nameWaterfallPointSize)->load();
        float waterfallDisplacement = apvts.getRawParameterValue(Parameters::nameWaterfallDisplacement)->load();

        if (waterfallSubdivisions != lastWaterfallSubdivisions)
        {
            waterfallRenderer->setSubdivisions(waterfallSubdivisions);
            lastWaterfallSubdivisions = waterfallSubdivisions;
        }
        if (std::abs(waterfallSize - lastWaterfallSize) > 0.001f)
        {
            waterfallRenderer->setSize(waterfallSize);
            lastWaterfallSize = waterfallSize;
        }
        waterfallRenderer->setPointSize(waterfallPointSize);
        waterfallRenderer->setDisplacementScale(waterfallDisplacement);
    }

    float colorSeed = apvts.getRawParameterValue(Parameters::nameColorSeed)->load() * 0.01f;
    renderer2D->setColorSeed(colorSeed);
    if (cubeRenderer != nullptr)      cubeRenderer->setColorSeed(colorSeed);
    if (waterfallRenderer != nullptr) waterfallRenderer->setColorSeed(colorSeed);

    juce::String newTexturePath = sharedAudioData.consumeTexturePathIfChanged(lastSeenTextureVersion);
    if (newTexturePath.isNotEmpty())
    {
        juce::File file(newTexturePath);
        if (file.existsAsFile())
            renderer2D->loadTexture(file);
    }
}

void OpenGLComponent::renderOpenGL()
{
    pullParameters();

    if (renderer2D != nullptr)
    {
        auto scale = (float)m_GLContext.getRenderingScale();
        int pixelWidth = juce::roundToInt((float)getWidth() * scale);
        int pixelHeight = juce::roundToInt((float)getHeight() * scale);

        switch (getActiveView())
        {
        case ActiveView::Grid2D: if (renderer2D) renderer2D->render(pixelWidth, pixelHeight); break;
        case ActiveView::Waterfall3D:if (waterfallRenderer) waterfallRenderer->render(pixelWidth, pixelHeight); break;
        case ActiveView::Cube3D:if (cubeRenderer) cubeRenderer->render(pixelWidth, pixelHeight); break;
        }
    }
}

void OpenGLComponent::openGLContextClosing()
{
    renderer2D.reset();
    cubeRenderer.reset();
    waterfallRenderer.reset();
}

void OpenGLComponent::resized()
{}

void OpenGLComponent::requestTexture(const juce::File& imageFile)
{
    sharedAudioData.requestTexturePath(imageFile.getFullPathName());
}

void OpenGLComponent::mouseDown(const juce::MouseEvent& e)
{
    if (e.mods.isLeftButtonDown())
    {
        m_LastMousePos = e.getPosition();
        setMouseCursor(juce::MouseCursor::NoCursor);
    }
}

void OpenGLComponent::mouseUp(const juce::MouseEvent& e)
{
    setMouseCursor(juce::MouseCursor::NormalCursor);
}

void OpenGLComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (e.mods.isLeftButtonDown() && renderer2D != nullptr)
    {
        if (auto* camera = getActiveCamera())
        {
            juce::Point<int> currentMousePos = e.getPosition();

            float deltaX = static_cast<float>(currentMousePos.x - m_LastMousePos.x);
            float deltaY = static_cast<float>(currentMousePos.y - m_LastMousePos.y);

            camera->ProcessMouseOrbit(glm::vec3(0.0f), deltaX, deltaY);

            m_LastMousePos = currentMousePos;
        }
    }
}

OpenGLComponent::ActiveView OpenGLComponent::getActiveView() const
{
    int dim = (int)apvts.getRawParameterValue(Parameters::nameDimensionMode)->load();
    if (dim == 0)
        return ActiveView::Grid2D;

    int m3 = (int)apvts.getRawParameterValue(Parameters::nameMode3D)->load();
    return (m3 == 0) ? ActiveView::Waterfall3D : ActiveView::Cube3D;
}

Camera* OpenGLComponent::getActiveCamera()
{
    switch (getActiveView())
    {
    case ActiveView::Grid2D: return renderer2D ? renderer2D->getCamera() : nullptr;
    case ActiveView::Waterfall3D: return waterfallRenderer ? waterfallRenderer->getCamera() : nullptr;
    case ActiveView::Cube3D: return cubeRenderer ? cubeRenderer->getCamera() : nullptr;
    }
    return nullptr;
}