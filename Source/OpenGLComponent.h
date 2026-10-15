#pragma once

#include "JuceHeader.h"
#include <juce_opengl/juce_opengl.h>

#include "SquareGridRenderer.h"
#include "CubeRenderer.h"
#include "WaterfallRenderer.h"
#include "SquareVisual.h"
#include "SharedAudioData.h"
#include "Parameters.h"

class OpenGLComponent : public juce::Component, public juce::OpenGLRenderer, private juce::Timer
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
    void timerCallback() override;

    juce::OpenGLContext m_GLContext;

    SquareVisual& visual;
    SharedAudioData& sharedAudioData;
    AudioProcessorValueTreeState& apvts;

    std::unique_ptr<SquareGridRenderer> renderer2D;
    std::unique_ptr<CubeRenderer> cubeRenderer;
    std::unique_ptr<WaterfallRenderer> waterfallRenderer;

    juce::Point<int> m_LastMousePos;

    int lastSeenTextureVersion = 0;

    struct GeometryState
    {
        int   subdivisions = -1;
        float size = -1.0f;
    };

    template <typename Renderer>
    static void applyGeometry(Renderer& renderer, int subdivisions, float size, GeometryState& last)
    {
        if (subdivisions != last.subdivisions)
        {
            renderer.setSubdivisions(subdivisions);
            last.subdivisions = subdivisions;
        }
        if (std::abs(size - last.size) > 0.001f)
        {
            renderer.setSize(size);
            last.size = size;
        }
    }

    GeometryState last2D, lastCube, lastWaterfall;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenGLComponent)
};