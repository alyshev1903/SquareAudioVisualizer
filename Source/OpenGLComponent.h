#pragma once

#include "JuceHeader.h"
#include <juce_opengl/juce_opengl.h>
#include <windows.h>
#include <GL/gl.h>

#include "SquareGridRenderer.h"
#include "SquareVisual.h"
#include "SharedAudioData.h"
#include "Parameters.h"

class OpenGLComponent : public juce::Component, public juce::OpenGLRenderer
{
public:
    OpenGLComponent(SquareVisual& visualToRender, SharedAudioData& sharedData, AudioProcessorValueTreeState& stateToRead);
    ~OpenGLComponent() override;

    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    void resized() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& event) override;

    void requestTexture(const juce::File& imageFile);

private:
    void pullParameters();

    juce::OpenGLContext m_GLContext;

    SquareVisual& visual;
    SharedAudioData& sharedAudioData;
    AudioProcessorValueTreeState& apvts;

    std::unique_ptr<SquareGridRenderer> renderer;

    juce::Point<int> m_LastMousePos;

    juce::CriticalSection textureLock;
    juce::File            pendingTextureFile;
    juce::File            lastLoadedTextureFile;

    int   lastSubdivisions = -1;
    float lastSize = -1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenGLComponent)
};