#include "OpenGLWindow.h"

OpenGLWindow::OpenGLWindow(SquareAudioVisualizerAudioProcessor& p, AudioProcessorValueTreeState& vts, SquareVisual& s)
    : DocumentWindow("OpenGL Window", juce::Colours::lightgrey, DocumentWindow::allButtons), squareVisual(s), audioProcessor(p)
{
    openGLComponent.reset(new OpenGLComponent(squareVisual, audioProcessor.getSharedAudioData(), vts));
    setUsingNativeTitleBar(true);
    openGLComponent->setSize(960, 540);
    setContentNonOwned(openGLComponent.get(), true);
    setResizable(true, false);
    setSize(960, 540);
    setVisible(true);
}

OpenGLWindow::~OpenGLWindow()
{
    audioProcessor.setWindowOpened(false);
    openGLComponent.reset();
}

void OpenGLWindow::closeButtonPressed()
{
    if (onCloseButtonPressed)
        onCloseButtonPressed();
}

void OpenGLWindow::resized()
{
    if (openGLComponent != nullptr)
        openGLComponent->setBounds(getLocalBounds());
}