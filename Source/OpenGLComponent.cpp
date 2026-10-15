#include "OpenGLComponent.h"
#include "vendor/glm/glm.hpp"
#include <cmath>

using namespace ::juce::gl;

OpenGLComponent::OpenGLComponent(SquareVisual& visualToRender, SharedAudioData& sharedData, AudioProcessorValueTreeState& stateToRead)
    : visual(visualToRender), sharedAudioData(sharedData), apvts(stateToRead)
{
    m_GLContext.setOpenGLVersionRequired(juce::OpenGLContext::openGL3_2);
    m_GLContext.setRenderer(this);
    m_GLContext.setComponentPaintingEnabled(false);
    m_GLContext.setContinuousRepainting(false);
    m_GLContext.attachTo(*this);

    startTimerHz(Parameters::renderFps);
}

OpenGLComponent::~OpenGLComponent()
{
    stopTimer();
    m_GLContext.detach();
}

void OpenGLComponent::timerCallback()
{
    if (isShowing())
        m_GLContext.triggerRepaint();
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

    auto param = [this](const juce::String& id)
    {
        return apvts.getRawParameterValue(id)->load();
    };

    auto forEachRenderer = [this](auto&& apply)
    {
        apply(*renderer2D);
        if (cubeRenderer != nullptr)      apply(*cubeRenderer);
        if (waterfallRenderer != nullptr) apply(*waterfallRenderer);
    };

    renderer2D->setZcrEffectEnabled(param(Parameters::nameZcrEnabled) > 0.5f);
    renderer2D->setPitchEffectEnabled(param(Parameters::namePitchEnabled) > 0.5f);
    renderer2D->setUseTexture((int)param(Parameters::namePointOrTexture) == 1);
    renderer2D->setPointSize(param(Parameters::namePointSize));
    applyGeometry(*renderer2D, (int)param(Parameters::nameSubdivision), param(Parameters::nameSize), last2D);

    const float echoAmount = param(Parameters::nameEchoAmount) * 0.01f;
    const float echoDelay = param(Parameters::nameEchoDelay);

    if (cubeRenderer != nullptr)
    {
        applyGeometry(*cubeRenderer, (int)param(Parameters::nameCubeSubdivision), param(Parameters::nameCubeSize), lastCube);
        cubeRenderer->setPointSize(param(Parameters::nameCubePointSize));
        cubeRenderer->setDisplacementScale(param(Parameters::nameCubeDisplacement));
        cubeRenderer->setRotationSpeed(param(Parameters::nameCubeRotationSpeed) * 0.001f);
        cubeRenderer->setEchoAmount(echoAmount);
        cubeRenderer->setEchoDelay(echoDelay);
    }

    if (waterfallRenderer != nullptr)
    {
        applyGeometry(*waterfallRenderer, (int)param(Parameters::nameWaterfallSubdivision), param(Parameters::nameWaterfallSize), lastWaterfall);
        waterfallRenderer->setPointSize(param(Parameters::nameWaterfallPointSize));
        waterfallRenderer->setDisplacementScale(param(Parameters::nameWaterfallDisplacement));
        waterfallRenderer->setEchoAmount(echoAmount);
        waterfallRenderer->setEchoDelay(echoDelay);
    }

    const float colorSeed = param(Parameters::nameColorSeed) * 0.01f;
    const float brightnessAmount = param(Parameters::nameBrightness) * 0.01f;
    const int   lfoTarget = (int)param(Parameters::nameLfoTarget);

    forEachRenderer([&](auto& renderer)
    {
        renderer.setColorSeed(colorSeed);
        renderer.setBrightnessAmount(brightnessAmount);
        renderer.setLfoTarget(lfoTarget);
    });

    renderer2D->setWaveAmount(param(Parameters::nameSparks) * 0.01f);

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