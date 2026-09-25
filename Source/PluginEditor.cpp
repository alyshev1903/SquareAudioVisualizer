#include "PluginProcessor.h"
#include "PluginEditor.h"

SquareAudioVisualizerAudioProcessorEditor::SquareAudioVisualizerAudioProcessorEditor(SquareAudioVisualizerAudioProcessor& p, AudioProcessorValueTreeState& vts, SquareVisual& sv)
    : audioProcessor(p), valueTreeState(vts), squareVisual(sv),
    m_GLComponent(sv, p.getSharedAudioData(), vts)
{
    setLookAndFeel(&Theme);

    auto setupKnob = [this](juce::Slider& knob, juce::Label& label, const juce::String& text)
    {
        knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        knob.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        knob.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(knob);

        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::Font(11.0f));
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    };

    setupKnob(subdivisionsSlider, subdivisionsLabel, "Subdivisions");
    setupKnob(sizeSlider, sizeLabel, "Size");
    setupKnob(pointSizeSlider, pointSizeLabel, "Point size");

    setupKnob(cubeSubdivisionsSlider, cubeSubdivisionsLabel, "Subdivisions");
    setupKnob(cubeSizeSlider, cubeSizeLabel, "Size");
    setupKnob(cubePointSizeSlider, cubePointSizeLabel, "Point size");
    setupKnob(cubeDisplacementSlider, cubeDisplacementLabel, "Displacement");
    setupKnob(cubeRotationSpeedSlider, cubeRotationSpeedLabel, "Rotation");

    setupKnob(waterfallSubdivisionsSlider, waterfallSubdivisionsLabel, "Subdivisions");
    setupKnob(waterfallSizeSlider, waterfallSizeLabel, "Size");
    setupKnob(waterfallPointSizeSlider, waterfallPointSizeLabel, "Point size");
    setupKnob(waterfallDisplacementSlider, waterfallDisplacementLabel, "Displacement");

    setupKnob(colorSeedSlider, colorSeedLabel, "Color");
    setupKnob(inputLevelSlider, inputLevelLabel, "Input Level");

    pointOrTextureBox.addItemList(Parameters::PointOrTextureOptions, 1);
    addAndMakeVisible(pointOrTextureBox);

    addAndMakeVisible(loadTextureButton);
    loadTextureButton.onClick = [this] { loadTextureButtonClicked(); };

    dimensionModeBox.addItemList(Parameters::DimensionModeOptions, 1);
    addAndMakeVisible(dimensionModeBox);
    mode3DBox.addItemList(Parameters::Mode3DOptions, 1);
    addAndMakeVisible(mode3DBox);

    addAndMakeVisible(zcrToggle);
    addAndMakeVisible(pitchToggle);

    addAndMakeVisible(OpenGLWindowButton);
    OpenGLWindowButton.onClick = [this]
    {
        if (!audioProcessor.getWindowOpened())
        {
            openOpenGLWindow();
            audioProcessor.setWindowOpened(true);
        }
    };

    zcrToggleAttachment = std::make_unique<ButtonAttachment>(valueTreeState, Parameters::nameZcrEnabled, zcrToggle);
    pitchToggleAttachment = std::make_unique<ButtonAttachment>(valueTreeState, Parameters::namePitchEnabled, pitchToggle);
    subdivisionsAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameSubdivision, subdivisionsSlider);
    sizeAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameSize, sizeSlider);
    pointSizeAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::namePointSize, pointSizeSlider);
    pointOrTextureAttachment = std::make_unique<ComboBoxAttachment>(valueTreeState, Parameters::namePointOrTexture, pointOrTextureBox);
    inputLevelAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameInputLevel, inputLevelSlider);
    dimensionModeAttachment = std::make_unique<ComboBoxAttachment>(valueTreeState, Parameters::nameDimensionMode, dimensionModeBox);
    mode3DAttachment = std::make_unique<ComboBoxAttachment>(valueTreeState, Parameters::nameMode3D, mode3DBox);

    cubeSubdivisionsAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameCubeSubdivision, cubeSubdivisionsSlider);
    cubeSizeAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameCubeSize, cubeSizeSlider);
    cubePointSizeAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameCubePointSize, cubePointSizeSlider);
    cubeDisplacementAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameCubeDisplacement, cubeDisplacementSlider);
    cubeRotationSpeedAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameCubeRotationSpeed, cubeRotationSpeedSlider);

    waterfallSubdivisionsAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameWaterfallSubdivision, waterfallSubdivisionsSlider);
    waterfallSizeAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameWaterfallSize, waterfallSizeSlider);
    waterfallPointSizeAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameWaterfallPointSize, waterfallPointSizeSlider);
    waterfallDisplacementAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameWaterfallDisplacement, waterfallDisplacementSlider);

    colorSeedAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameColorSeed, colorSeedSlider);
    Theme.setAccentHue((float)colorSeedSlider.getValue());

    colorSeedSlider.onValueChange = [this]
    {
        Theme.setAccentHue((float)colorSeedSlider.getValue());
        repaint();
    };

    inputLevelSlider.setTextValueSuffix(" dB");

    dimensionModeBox.onChange = [this] { updateControlVisibility(); };
    mode3DBox.onChange = [this] { updateControlVisibility(); };
    updateControlVisibility();

    addAndMakeVisible(m_GLComponent);
    setSize(960, 760);
}

SquareAudioVisualizerAudioProcessorEditor::~SquareAudioVisualizerAudioProcessorEditor()
{
	setLookAndFeel(nullptr);
}

void SquareAudioVisualizerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void SquareAudioVisualizerAudioProcessorEditor::updateControlVisibility()
{
    int dim = (int)*valueTreeState.getRawParameterValue(Parameters::nameDimensionMode);
    int m3 = (int)*valueTreeState.getRawParameterValue(Parameters::nameMode3D);

    mode3DBox.setVisible(dim == 1);

    bool show2D = (dim == 0);
    bool showWaterfall = (dim == 1 && m3 == 0);
    bool showCube = (dim == 1 && m3 == 1);

    for (auto* c : { (juce::Component*)&subdivisionsSlider, (juce::Component*)&sizeSlider, (juce::Component*)&pointSizeSlider,
        (juce::Component*)&pointOrTextureBox, (juce::Component*)&loadTextureButton,(juce::Component*)&zcrToggle, (juce::Component*)&pitchToggle,
        (juce::Component*)&subdivisionsLabel, (juce::Component*)&sizeLabel, (juce::Component*)&pointSizeLabel })
        c->setVisible(show2D);

    for (auto* c : { (juce::Component*)&cubeSubdivisionsSlider, (juce::Component*)&cubeSizeSlider, (juce::Component*)&cubePointSizeSlider,
        (juce::Component*)&cubeDisplacementSlider, (juce::Component*)&cubeRotationSpeedSlider,
        (juce::Component*)&cubeSubdivisionsLabel, (juce::Component*)&cubeSizeLabel, (juce::Component*)&cubePointSizeLabel,
        (juce::Component*)&cubeDisplacementLabel, (juce::Component*)&cubeRotationSpeedLabel })
        c->setVisible(showCube);

    for (auto* c : { (juce::Component*)&waterfallSubdivisionsSlider, (juce::Component*)&waterfallSizeSlider, (juce::Component*)&waterfallPointSizeSlider,
        (juce::Component*)&waterfallDisplacementSlider,(juce::Component*)&waterfallSubdivisionsLabel, (juce::Component*)&waterfallSizeLabel, (juce::Component*)&waterfallPointSizeLabel,
        (juce::Component*)&waterfallDisplacementLabel })
        c->setVisible(showWaterfall);

    resized();
}

void SquareAudioVisualizerAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto controlsArea = bounds.removeFromBottom(268).reduced(8);
    m_GLComponent.setBounds(bounds);

    const int knobSize = 52;
    const int knobGap = 30;
    const int bigKnobSize = 84;
    const int labelHeight = 16;

    auto layoutKnobRowCentered = [&](juce::Rectangle<int> area, std::initializer_list<std::pair<juce::Slider*, juce::Label*>> knobs)
    {
        int count = (int)knobs.size();
        if (count == 0) return;
        int totalWidth = count * knobSize + (count - 1) * knobGap;
        int x = area.getCentreX() - totalWidth / 2;
        for (auto& [knob, label] : knobs)
        {
            knob->setBounds(x, area.getY(), knobSize, knobSize);
            label->setBounds(x - (knobGap / 2), area.getY() - labelHeight - 2, knobSize + knobGap, labelHeight);
            x += knobSize + knobGap;
        }
    };

    auto modeRow = controlsArea.removeFromTop(28);
    dimensionModeBox.setBounds(modeRow.removeFromLeft(130));
    modeRow.removeFromLeft(8);
    mode3DBox.setBounds(modeRow.removeFromLeft(150));

    pitchToggle.setBounds(modeRow.removeFromRight(90));
    modeRow.removeFromRight(8);
    zcrToggle.setBounds(modeRow.removeFromRight(90));
    modeRow.removeFromRight(20);
    loadTextureButton.setBounds(modeRow.removeFromRight(150));
    modeRow.removeFromRight(10);
    pointOrTextureBox.setBounds(modeRow.removeFromRight(150));

    controlsArea.removeFromTop(labelHeight + 6);

    auto knobRow = controlsArea.removeFromTop(knobSize);

    int dim = (int)*valueTreeState.getRawParameterValue(Parameters::nameDimensionMode);
    int m3 = (int)*valueTreeState.getRawParameterValue(Parameters::nameMode3D);

    if (dim == 0)
        layoutKnobRowCentered(knobRow, { {&subdivisionsSlider,&subdivisionsLabel}, {&sizeSlider,&sizeLabel},
            {&pointSizeSlider,&pointSizeLabel}, {&colorSeedSlider,&colorSeedLabel} });
    else if (m3 == 1)
        layoutKnobRowCentered(knobRow, { {&cubeSubdivisionsSlider,&cubeSubdivisionsLabel}, {&cubeSizeSlider,&cubeSizeLabel},
            {&cubePointSizeSlider,&cubePointSizeLabel}, {&cubeDisplacementSlider,&cubeDisplacementLabel},
            {&cubeRotationSpeedSlider,&cubeRotationSpeedLabel},{&colorSeedSlider,&colorSeedLabel} });
    else
        layoutKnobRowCentered(knobRow, { {&waterfallSubdivisionsSlider,&waterfallSubdivisionsLabel}, {&waterfallSizeSlider,&waterfallSizeLabel},
            {&waterfallPointSizeSlider,&waterfallPointSizeLabel}, {&waterfallDisplacementSlider,&waterfallDisplacementLabel},
            {&colorSeedSlider,&colorSeedLabel}});

    controlsArea.removeFromTop(20);

    auto bigKnobRow = controlsArea.removeFromTop(bigKnobSize);
    inputLevelSlider.setBounds(bigKnobRow.withSizeKeepingCentre(bigKnobSize, bigKnobSize));
    inputLevelLabel.setBounds(bigKnobRow.withSizeKeepingCentre(bigKnobSize + 40, labelHeight).translated(0, -bigKnobSize / 2 - labelHeight));

    controlsArea.removeFromTop(14);

    auto bottomRow = controlsArea.removeFromTop(28);
    OpenGLWindowButton.setBounds(bottomRow.removeFromLeft(180));
}

void SquareAudioVisualizerAudioProcessorEditor::loadTextureButtonClicked()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Scegli un'immagine per la texture",
        juce::File(),
        "*.png;*.jpg;*.jpeg");

    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& chooser)
    {
        auto file = chooser.getResult();
        if (file.existsAsFile())
            m_GLComponent.requestTexture(file);
    });
}

void SquareAudioVisualizerAudioProcessorEditor::openOpenGLWindow()
{
    openGLWindow = std::make_unique<OpenGLWindow>(audioProcessor, valueTreeState, squareVisual);
    openGLWindow->onCloseButtonPressed = [this] { openGLWindow.reset(); };
}

WrappedPluginEditor::WrappedPluginEditor(SquareAudioVisualizerAudioProcessor& p, AudioProcessorValueTreeState& vts, SquareVisual& s)
    : AudioProcessorEditor(&p), editorComponent(p, vts, s)
{
    setSize(960, 760);
    addAndMakeVisible(editorComponent);
}

WrappedPluginEditor::~WrappedPluginEditor()
{}

void WrappedPluginEditor::resized()
{
    editorComponent.setBounds(getLocalBounds());
}