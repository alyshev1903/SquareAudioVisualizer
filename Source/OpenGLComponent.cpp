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
    renderer = std::make_unique<SquareGridRenderer>(visual, sharedAudioData);
}

void OpenGLComponent::pullParameters()
{
    if (renderer == nullptr)
        return;

    int   subdivisions = (int)apvts.getRawParameterValue(Parameters::nameSubdivision)->load();
    float size = apvts.getRawParameterValue(Parameters::nameSize)->load();
    float pointSize = apvts.getRawParameterValue(Parameters::namePointSize)->load();
    int   pointOrTexture = (int)apvts.getRawParameterValue(Parameters::namePointOrTexture)->load();
    /*float translationX = apvts.getRawParameterValue(Parameters::TranslationX)->load();
    float translationY = apvts.getRawParameterValue(Parameters::TranslationY)->load();
    float translationZ = apvts.getRawParameterValue(Parameters::TranslationZ)->load();
    float sensitivity = apvts.getRawParameterValue(Parameters::Sensitivity)->load();*/
    bool zcrEnabled = apvts.getRawParameterValue(Parameters::nameZcrEnabled)->load() > 0.5f;
    bool pitchEnabled = apvts.getRawParameterValue(Parameters::namePitchEnabled)->load() > 0.5f;

    renderer->setZcrEffectEnabled(zcrEnabled);
    renderer->setPitchEffectEnabled(pitchEnabled);

    if (subdivisions != lastSubdivisions)
    {
        renderer->setSubdivisions(subdivisions);
        lastSubdivisions = subdivisions;
    }
    if (std::abs(size - lastSize) > 0.001f)
    {
        renderer->setSize(size);
        lastSize = size;
    }

    renderer->setPointSize(pointSize);
    renderer->setUseTexture(pointOrTexture == 1);
    //renderer->setTranslation(glm::vec3(translationX, translationY, translationZ));

    if (auto* camera = renderer->getCamera())
        //camera->GetSensitivity() = sensitivity;

    {
        juce::String newTexturePath = sharedAudioData.consumeTexturePathIfChanged(lastSeenTextureVersion);
        if (newTexturePath.isNotEmpty())
        {
            juce::File file(newTexturePath);
            if (file.existsAsFile())
                renderer->loadTexture(file);
        }
    }

}

void OpenGLComponent::renderOpenGL()
{
    pullParameters();

    if (renderer != nullptr)
    {
        auto scale = (float)m_GLContext.getRenderingScale();
        int pixelWidth = juce::roundToInt((float)getWidth() * scale);
        int pixelHeight = juce::roundToInt((float)getHeight() * scale);
        renderer->render(pixelWidth, pixelHeight);
    }
}

void OpenGLComponent::openGLContextClosing()
{
    renderer.reset();
}

void OpenGLComponent::resized()
{}

void OpenGLComponent::requestTexture(const juce::File& imageFile)
{
    sharedAudioData.requestTexturePath(imageFile.getFullPathName());
}

void OpenGLComponent::mouseDown(const juce::MouseEvent& e)
{
    if (e.mods.isRightButtonDown())
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
    if (e.mods.isRightButtonDown() && renderer != nullptr)
    {
        if (auto* camera = renderer->getCamera())
        {
            juce::Point<int> currentMousePos = e.getPosition();

            float deltaX = static_cast<float>(currentMousePos.x - m_LastMousePos.x);
            float deltaY = static_cast<float>(currentMousePos.y - m_LastMousePos.y);

            camera->ProcessMouseOrbit(renderer->getTranslation(), deltaX, deltaY);

            m_LastMousePos = currentMousePos;
        }
    }
}