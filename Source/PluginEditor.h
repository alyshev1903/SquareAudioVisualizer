#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "OpenGLWindow.h"
#include "OpenGLComponent.h"
#include "FilterDisplay.h"
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
    FilterDisplay filterDisplay;

    void openOpenGLWindow();
    void loadTextureButtonClicked();

    void updateControlVisibility();
    void drawGroup(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& title);

    juce::Slider subdivisionsSlider, sizeSlider, pointSizeSlider, inputLevelSlider;
    juce::Label  subdivisionsLabel, sizeLabel, pointSizeLabel, inputLevelLabel;

    juce::Slider cubeSubdivisionsSlider, cubeSizeSlider, cubePointSizeSlider, cubeDisplacementSlider, cubeRotationSpeedSlider;
    juce::Label  cubeSubdivisionsLabel, cubeSizeLabel, cubePointSizeLabel, cubeDisplacementLabel, cubeRotationSpeedLabel;

    juce::Slider waterfallSubdivisionsSlider, waterfallSizeSlider, waterfallPointSizeSlider, waterfallDisplacementSlider;
    juce::Label  waterfallSubdivisionsLabel, waterfallSizeLabel, waterfallPointSizeLabel, waterfallDisplacementLabel;

    juce::Slider colorSeedSlider;
    juce::Label  colorSeedLabel;

    juce::Slider   lfoRateSlider;
    juce::Label    lfoRateLabel, lfoTargetLabel;

    juce::Slider   gateThresholdSlider;
    juce::Label    gateThresholdLabel, gateTargetLabel;

    juce::Slider lowCutSlider, highCutSlider;
    juce::Label  lowCutLabel, highCutLabel;

    juce::Slider brightnessSlider;
    juce::Label  brightnessLabel;

    juce::Slider echoAmountSlider, echoDelaySlider, sparksSlider;
    juce::Label  echoAmountLabel, echoDelayLabel, sparksLabel;

    juce::ComboBox pointOrTextureBox;
    juce::ComboBox dimensionModeBox;
    juce::ComboBox mode3DBox;
    juce::ComboBox lfoTargetBox, lfoShapeBox, lfoDivisionBox;
    juce::ComboBox gateTargetBox;
    juce::TextButton loadTextureButton{ "Load texture..." };
    juce::TextButton OpenGLWindowButton{ "Open external Window" };
    juce::ToggleButton zcrToggle{ "Onset" };
    juce::ToggleButton pitchToggle{ "Pitch" };
    juce::ToggleButton lfoSyncToggle{ "Sync" };

    juce::Rectangle<int> visualGroupArea, audioGroupArea, lfoGroupArea, gateGroupArea;
    juce::String visualTitle;

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

    std::unique_ptr<SliderAttachment>   lfoRateAttachment;
    std::unique_ptr<ComboBoxAttachment> lfoTargetAttachment, lfoShapeAttachment, lfoDivisionAttachment;
    std::unique_ptr<ButtonAttachment>   lfoSyncAttachment;

    std::unique_ptr<SliderAttachment>   gateThresholdAttachment;
    std::unique_ptr<ComboBoxAttachment> gateTargetAttachment;

    std::unique_ptr<SliderAttachment> lowCutAttachment, highCutAttachment;
    std::unique_ptr<SliderAttachment> brightnessAttachment;

    std::unique_ptr<SliderAttachment> echoAmountAttachment, echoDelayAttachment, sparksAttachment;

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