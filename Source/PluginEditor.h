#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "OpenGLWindow.h"
#include "OpenGLComponent.h"
#include "Parameters.h"
#include "MyTheme.h"

class SquareAudioVisualizerAudioProcessorEditor : public juce::Component
{
public:
    SquareAudioVisualizerAudioProcessorEditor(SquareAudioVisualizerAudioProcessor&, AudioProcessorValueTreeState&, SquareVisual&);
    ~SquareAudioVisualizerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SquareAudioVisualizerAudioProcessor& audioProcessor;
    AudioProcessorValueTreeState& valueTreeState;
    SquareVisual& squareVisual;

    OpenGLComponent m_GLComponent;

    void openOpenGLWindow();
    void loadTextureButtonClicked();

    void updateControlVisibility();

    juce::Slider subdivisionsSlider, sizeSlider, pointSizeSlider, inputLevelSlider;
    juce::Label  subdivisionsLabel, sizeLabel, pointSizeLabel, inputLevelLabel;

    juce::Slider cubeSubdivisionsSlider, cubeSizeSlider, cubePointSizeSlider, cubeDisplacementSlider, cubeRotationSpeedSlider;
    juce::Label  cubeSubdivisionsLabel, cubeSizeLabel, cubePointSizeLabel, cubeDisplacementLabel, cubeRotationSpeedLabel;

    juce::Slider waterfallSubdivisionsSlider, waterfallSizeSlider, waterfallPointSizeSlider, waterfallDisplacementSlider;
    juce::Label  waterfallSubdivisionsLabel, waterfallSizeLabel, waterfallPointSizeLabel, waterfallDisplacementLabel;

    juce::Slider colorSeedSlider;
    juce::Label  colorSeedLabel;

    juce::ComboBox pointOrTextureBox;
    juce::ComboBox dimensionModeBox;
    juce::ComboBox mode3DBox;
    juce::TextButton loadTextureButton{ "Load texture..." };
	juce::TextButton OpenGLWindowButton{ "Open external Window" };
    juce::ToggleButton zcrToggle{ "ZCR"};
    juce::ToggleButton pitchToggle{ "Pitch" };

    using ButtonAttachment = AudioProcessorValueTreeState::ButtonAttachment;
    using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<ButtonAttachment> zcrToggleAttachment, pitchToggleAttachment;
    std::unique_ptr<SliderAttachment> subdivisionsAttachment, sizeAttachment, pointSizeAttachment, inputLevelAttachment;
    std::unique_ptr<ComboBoxAttachment> pointOrTextureAttachment;
    std::unique_ptr<ComboBoxAttachment> dimensionModeAttachment, mode3DAttachment;
    std::unique_ptr<SliderAttachment> cubeSubdivisionsAttachment, cubeSizeAttachment, cubePointSizeAttachment, cubeDisplacementAttachment, cubeRotationSpeedAttachment;
    std::unique_ptr<SliderAttachment> waterfallSubdivisionsAttachment, waterfallSizeAttachment, waterfallPointSizeAttachment, waterfallDisplacementAttachment;
    std::unique_ptr<SliderAttachment> colorSeedAttachment;

    std::unique_ptr<OpenGLWindow> openGLWindow;

    std::unique_ptr<juce::FileChooser> fileChooser;

	MyTheme Theme;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SquareAudioVisualizerAudioProcessorEditor)
};

class WrappedPluginEditor : public AudioProcessorEditor
{
public:
    WrappedPluginEditor(SquareAudioVisualizerAudioProcessor& p, AudioProcessorValueTreeState& vts, SquareVisual& s);
    ~WrappedPluginEditor();
    void resized() override;

private:
    SquareAudioVisualizerAudioProcessorEditor editorComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WrappedPluginEditor)
};