#include "OpenGLWindow.h"

OpenGLWindow::OpenGLWindow(SquareAudioVisualizerAudioProcessor& p, AudioProcessorValueTreeState& vts, SquareVisuals& s)
    : DocumentWindow("OpenGL Window", juce::Colours::lightgrey, DocumentWindow::allButtons), squareVisuals(s), audioProcessor(p)
{
    openGLComponent.reset(new OpenGLComponent(squareVisuals[0], audioProcessor.getSharedAudioData(), vts));
    setUsingNativeTitleBar(true);
    openGLComponent->setSize(960, 540);
    setContentOwned(openGLComponent.get(), true);
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
    delete this;
}

void OpenGLWindow::resized()
{
    if (openGLComponent != nullptr)
        openGLComponent->setBounds(getLocalBounds());
}