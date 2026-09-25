#pragma once

#include "JuceHeader.h"
#include <juce_opengl/juce_opengl.h>

#include "SquareGridRenderer.h"
#include "CubeRenderer.h"
#include "WaterfallRenderer.h"
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

    Camera* getActiveCamera();

private:
    enum class ActiveView { Grid2D, Waterfall3D, Cube3D };

    void pullParameters();
    ActiveView getActiveView() const;

    juce::OpenGLContext m_GLContext;

    SquareVisual& visual;
    SharedAudioData& sharedAudioData;
    AudioProcessorValueTreeState& apvts;

    std::unique_ptr<SquareGridRenderer> renderer2D;
    std::unique_ptr<CubeRenderer> cubeRenderer;
    std::unique_ptr<WaterfallRenderer> waterfallRenderer;

    juce::Point<int> m_LastMousePos;

    int lastSeenTextureVersion = 0;

    int   lastSubdivisions = -1;
    float lastSize = -1.0f;
    int   lastCubeSubdivisions = -1;
    float lastCubeSize = -1.0f;
    int   lastWaterfallSubdivisions = -1;
    float lastWaterfallSize = -1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenGLComponent)
};