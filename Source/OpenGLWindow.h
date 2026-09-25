#pragma once

#include <juce_opengl/juce_opengl.h>

#include "PluginProcessor.h"
#include "OpenGLComponent.h"

class OpenGLWindow : public juce::DocumentWindow
{
public:
    OpenGLWindow(SquareAudioVisualizerAudioProcessor& p, AudioProcessorValueTreeState& vts, SquareVisual& v);
    ~OpenGLWindow() override;

    std::function<void()> onCloseButtonPressed;

private:
    void closeButtonPressed() override;
    void resized()            override;

    SquareVisual& squareVisual;
    std::unique_ptr<OpenGLComponent> openGLComponent;
    SquareAudioVisualizerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenGLWindow)
};