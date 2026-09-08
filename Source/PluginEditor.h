#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "OpenGLWindow.h"
#include "OpenGLComponent.h"
#include "Parameters.h"
#include "MyTheme.h"

class SquareAudioVisualizerAudioProcessorEditor : public juce::Component, public::juce::Slider::Listener, public::ComboBox::Listener
{
public:
    SquareAudioVisualizerAudioProcessorEditor(SquareAudioVisualizerAudioProcessor&, AudioProcessorValueTreeState&, SquareVisuals&);
    ~SquareAudioVisualizerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SquareAudioVisualizerAudioProcessor& audioProcessor;
    AudioProcessorValueTreeState& valueTreeState;
    SquareVisuals& squareVisuals;

    OpenGLComponent m_GLComponent;

    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* box)  override;

    void openOpenGLWindow();
    void loadTextureButtonClicked();

    juce::TextButton openGLButton, v1_visibilityButton, v2_visibilityButton, bValues;

    juce::Slider subdivisionsSlider, sizeSlider, pointSizeSlider, inputLevelSlider;
    juce::Label  subdivisionsLabel, sizeLabel, pointSizeLabel, inputLevelLabel;
    juce::ComboBox   pointOrTextureBox;
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

    std::unique_ptr<juce::FileChooser> fileChooser;

	MyTheme Theme;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SquareAudioVisualizerAudioProcessorEditor)
};

class WrappedPluginEditor : public AudioProcessorEditor
{
public:
    WrappedPluginEditor(SquareAudioVisualizerAudioProcessor& p, AudioProcessorValueTreeState& vts, SquareVisuals& s);
    ~WrappedPluginEditor();
    void resized() override;

private:
    SquareAudioVisualizerAudioProcessorEditor editorComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WrappedPluginEditor)
};