#include "PluginProcessor.h"
#include "PluginEditor.h"

SquareAudioVisualizerAudioProcessorEditor::SquareAudioVisualizerAudioProcessorEditor(SquareAudioVisualizerAudioProcessor& p, AudioProcessorValueTreeState& vts, SquareVisual& sv)
    : audioProcessor(p), valueTreeState(vts), squareVisual(sv),
    m_GLComponent(sv, p.getSharedAudioData(), vts),
    filterDisplay(vts, p.getSharedAudioData())
{
    setLookAndFeel(&Theme);

    auto setupKnob = [this](juce::Slider& knob, juce::Label& label, const juce::String& text)
    {
        knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 70, 20);
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
    setupKnob(brightnessSlider, brightnessLabel, "Brightness");

    setupKnob(echoAmountSlider, echoAmountLabel, "Echo");
    setupKnob(echoDelaySlider, echoDelayLabel, "Echo delay");
    echoDelaySlider.setTextValueSuffix(" ms");
    setupKnob(sparksSlider, sparksLabel, "Sparks");

    setupKnob(inputLevelSlider, inputLevelLabel, "Input Level");
    setupKnob(lowCutSlider, lowCutLabel, "Low cut");
    setupKnob(highCutSlider, highCutLabel, "High cut");
    lowCutSlider.setTextValueSuffix(" Hz");
    highCutSlider.setTextValueSuffix(" Hz");

    setupKnob(lfoRateSlider, lfoRateLabel, "Rate");
    lfoRateSlider.setTextValueSuffix(" Hz");

    setupKnob(gateThresholdSlider, gateThresholdLabel, "Threshold");

    lfoTargetLabel.setText("LFO target", juce::dontSendNotification);
    lfoTargetLabel.setFont(juce::Font(11.0f));
    lfoTargetLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(lfoTargetLabel);

    lfoTargetBox.addItemList(Parameters::LfoTargetOptions, 1);
    addAndMakeVisible(lfoTargetBox);
    lfoShapeBox.addItemList(Parameters::LfoShapeOptions, 1);
    addAndMakeVisible(lfoShapeBox);
    lfoDivisionBox.addItemList(Parameters::LfoDivisionOptions, 1);
    addAndMakeVisible(lfoDivisionBox);
    addAndMakeVisible(lfoSyncToggle);

    gateTargetLabel.setText("Gate target", juce::dontSendNotification);
    gateTargetLabel.setFont(juce::Font(11.0f));
    gateTargetLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(gateTargetLabel);

    gateTargetBox.addItemList(Parameters::GateTargetOptions, 1);
    addAndMakeVisible(gateTargetBox);

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

    addAndMakeVisible(filterDisplay);

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

    lfoTargetAttachment = std::make_unique<ComboBoxAttachment>(valueTreeState, Parameters::nameLfoTarget, lfoTargetBox);
    lfoShapeAttachment = std::make_unique<ComboBoxAttachment>(valueTreeState, Parameters::nameLfoShape, lfoShapeBox);
    lfoDivisionAttachment = std::make_unique<ComboBoxAttachment>(valueTreeState, Parameters::nameLfoDivision, lfoDivisionBox);
    lfoRateAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameLfoRate, lfoRateSlider);
    lfoSyncAttachment = std::make_unique<ButtonAttachment>(valueTreeState, Parameters::nameLfoSync, lfoSyncToggle);

    gateTargetAttachment = std::make_unique<ComboBoxAttachment>(valueTreeState, Parameters::nameGateTarget, gateTargetBox);
    gateThresholdAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameGateThreshold, gateThresholdSlider);

    lowCutAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameFilterLowCut, lowCutSlider);
    highCutAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameFilterHighCut, highCutSlider);

    brightnessAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameBrightness, brightnessSlider);

    echoAmountAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameEchoAmount, echoAmountSlider);
    echoDelayAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameEchoDelay, echoDelaySlider);
    sparksAttachment = std::make_unique<SliderAttachment>(valueTreeState, Parameters::nameSparks, sparksSlider);

    Theme.setAccentHue((float)colorSeedSlider.getValue() * 0.01f);
    filterDisplay.setAccent(Theme.getAccent());

    colorSeedSlider.onValueChange = [this]
    {
        Theme.setAccentHue((float)colorSeedSlider.getValue() * 0.01f);
        filterDisplay.setAccent(Theme.getAccent());
        repaint();
    };

    inputLevelSlider.setTextValueSuffix(" dB");

    dimensionModeBox.onChange = [this] { updateControlVisibility(); };
    mode3DBox.onChange = [this] { updateControlVisibility(); };
    lfoSyncToggle.onClick = [this] { updateControlVisibility(); };
    updateControlVisibility();

    addAndMakeVisible(m_GLComponent);
    setSize(960, 920);
}

SquareAudioVisualizerAudioProcessorEditor::~SquareAudioVisualizerAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void SquareAudioVisualizerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    drawGroup(g, visualGroupArea, visualTitle);
    drawGroup(g, lfoGroupArea, "LFO");
    drawGroup(g, gateGroupArea, "GATE");
    drawGroup(g, audioGroupArea, "AUDIO - FILTER");
}

void SquareAudioVisualizerAudioProcessorEditor::drawGroup(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& title)
{
    if (area.isEmpty())
        return;

    auto frame = area.toFloat();

    g.setColour(juce::Colour(0xfff6f6f6));
    g.fillRoundedRectangle(frame, 8.0f);

    g.setColour(juce::Colour(0xffdcdcdc));
    g.drawRoundedRectangle(frame.reduced(0.5f), 8.0f, 1.0f);

    g.setColour(Theme.getAccent());
    g.setFont(juce::Font(11.0f).boldened());
    g.drawText(title, area.getX() + 12, area.getY() + 4, area.getWidth() - 24, 16, juce::Justification::centredLeft);
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
        (juce::Component*)&subdivisionsLabel, (juce::Component*)&sizeLabel, (juce::Component*)&pointSizeLabel, (juce::Component*)&sparksSlider, (juce::Component*)&sparksLabel })
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

    for (auto* c : { (juce::Component*)&echoAmountSlider, (juce::Component*)&echoDelaySlider,
    (juce::Component*)&echoAmountLabel, (juce::Component*)&echoDelayLabel })
        c->setVisible(dim == 1);

    const bool lfoSync = lfoSyncToggle.getToggleState();
    lfoRateSlider.setVisible(!lfoSync);
    lfoRateLabel.setVisible(!lfoSync);
    lfoDivisionBox.setVisible(lfoSync);

    resized();
    repaint();
}

void SquareAudioVisualizerAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto controlsArea = bounds.removeFromBottom(440).reduced(8);
    m_GLComponent.setBounds(bounds);

    const int slotWidth = 80;
    const int knobWidth = 72;
    const int knobHeight = 78;
    const int labelHeight = 14;
    const int titleHeight = 22;
    const int comboHeight = 28;

    auto placeKnob = [&](int centreX, int top, juce::Slider& knob, juce::Label& label, int width, int height)
    {
        label.setBounds(centreX - slotWidth / 2, top, slotWidth, labelHeight);
        knob.setBounds(centreX - width / 2, top + labelHeight, width, height);
    };

    auto layoutKnobRow = [&](juce::Rectangle<int> group, std::initializer_list<std::pair<juce::Slider*, juce::Label*>> knobs)
    {
        const int count = (int)knobs.size();
        int centreX = group.getCentreX() - (count * slotWidth) / 2 + slotWidth / 2;
        const int top = group.getY() + titleHeight;
        for (auto& [knob, label] : knobs)
        {
            placeKnob(centreX, top, *knob, *label, knobWidth, knobHeight);
            centreX += slotWidth;
        }
    };

    auto modeRow = controlsArea.removeFromTop(comboHeight);
    dimensionModeBox.setBounds(modeRow.removeFromLeft(130));
    modeRow.removeFromLeft(8);
    mode3DBox.setBounds(modeRow.removeFromLeft(150));

    OpenGLWindowButton.setBounds(modeRow.removeFromRight(170));
    modeRow.removeFromRight(16);
    pitchToggle.setBounds(modeRow.removeFromRight(80));
    modeRow.removeFromRight(8);
    zcrToggle.setBounds(modeRow.removeFromRight(80));
    modeRow.removeFromRight(16);
    loadTextureButton.setBounds(modeRow.removeFromRight(130));
    modeRow.removeFromRight(8);
    pointOrTextureBox.setBounds(modeRow.removeFromRight(110));

    controlsArea.removeFromTop(8);
    visualGroupArea = controlsArea.removeFromTop(120);

    int dim = (int)*valueTreeState.getRawParameterValue(Parameters::nameDimensionMode);
    int m3 = (int)*valueTreeState.getRawParameterValue(Parameters::nameMode3D);

    if (dim == 0)
    {
        visualTitle = "VISUAL - 2D";
        layoutKnobRow(visualGroupArea, { {&subdivisionsSlider,&subdivisionsLabel}, {&sizeSlider,&sizeLabel},
            {&pointSizeSlider,&pointSizeLabel}, {&colorSeedSlider,&colorSeedLabel}, {&brightnessSlider,&brightnessLabel}, {&sparksSlider,&sparksLabel} });
    }
    else if (m3 == 1)
    {
        visualTitle = "VISUAL - CUBE";
        layoutKnobRow(visualGroupArea, { {&cubeSubdivisionsSlider,&cubeSubdivisionsLabel}, {&cubeSizeSlider,&cubeSizeLabel},
            {&cubePointSizeSlider,&cubePointSizeLabel}, {&cubeDisplacementSlider,&cubeDisplacementLabel},
            {&cubeRotationSpeedSlider,&cubeRotationSpeedLabel}, {&colorSeedSlider,&colorSeedLabel}, {&brightnessSlider,&brightnessLabel},
            {&echoAmountSlider,&echoAmountLabel}, {&echoDelaySlider,&echoDelayLabel} });
    }
    else
    {
        visualTitle = "VISUAL - WATERFALL";
        layoutKnobRow(visualGroupArea, { {&waterfallSubdivisionsSlider,&waterfallSubdivisionsLabel}, {&waterfallSizeSlider,&waterfallSizeLabel},
            {&waterfallPointSizeSlider,&waterfallPointSizeLabel}, {&waterfallDisplacementSlider,&waterfallDisplacementLabel},
            {&colorSeedSlider,&colorSeedLabel}, {&brightnessSlider,&brightnessLabel}, {&echoAmountSlider,&echoAmountLabel}, {&echoDelaySlider,&echoDelayLabel} });
    }

    controlsArea.removeFromTop(8);
    auto rowC = controlsArea.removeFromTop(120);
    lfoGroupArea = rowC.removeFromLeft(520);
    rowC.removeFromLeft(8);
    gateGroupArea = rowC;

    const int lfoTop = lfoGroupArea.getY() + titleHeight;
    const int lfoLeft = lfoGroupArea.getX() + 12;

    placeKnob(lfoLeft + slotWidth / 2, lfoTop, lfoRateSlider, lfoRateLabel, knobWidth, knobHeight);
    lfoDivisionBox.setBounds(lfoLeft, lfoTop + labelHeight + (knobHeight - comboHeight) / 2, slotWidth, comboHeight);

    const int lfoRightX = lfoLeft + slotWidth + 16;
    juce::Rectangle<int> lfoRight(lfoRightX, lfoTop, lfoGroupArea.getRight() - 12 - lfoRightX, 80);
    lfoTargetLabel.setBounds(lfoRight.removeFromTop(labelHeight));
    lfoTargetBox.setBounds(lfoRight.removeFromTop(comboHeight));
    lfoRight.removeFromTop(8);
    auto lfoRow = lfoRight.removeFromTop(comboHeight);
    lfoShapeBox.setBounds(lfoRow.removeFromLeft(160));
    lfoRow.removeFromLeft(12);
    lfoSyncToggle.setBounds(lfoRow.removeFromLeft(100));

    const int gateTop = gateGroupArea.getY() + titleHeight;
    placeKnob(gateGroupArea.getRight() - 12 - slotWidth / 2, gateTop, gateThresholdSlider, gateThresholdLabel, knobWidth, knobHeight);

    juce::Rectangle<int> gateLeft(gateGroupArea.getX() + 12, gateTop, gateGroupArea.getWidth() - 24 - slotWidth - 16, 80);
    gateTargetLabel.setBounds(gateLeft.removeFromTop(labelHeight));
    gateTargetBox.setBounds(gateLeft.removeFromTop(comboHeight));

    controlsArea.removeFromTop(8);
    audioGroupArea = controlsArea;

    const int audioTop = audioGroupArea.getY() + titleHeight;
    int left = audioGroupArea.getX() + 12;
    const int right = audioGroupArea.getRight() - 12;

    inputLevelLabel.setBounds(left, audioTop, 100, labelHeight);
    inputLevelSlider.setBounds(left + 2, audioTop + labelHeight, 96, 92);
    left += 100 + 8;

    placeKnob(left + slotWidth / 2, audioTop, lowCutSlider, lowCutLabel, knobWidth, knobHeight);
    left += slotWidth + 8;

    placeKnob(right - slotWidth / 2, audioTop, highCutSlider, highCutLabel, knobWidth, knobHeight);

    filterDisplay.setBounds(left, audioTop, right - slotWidth - 8 - left, audioGroupArea.getBottom() - 8 - audioTop);
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
    setSize(960, 920);
    addAndMakeVisible(editorComponent);
}

WrappedPluginEditor::~WrappedPluginEditor()
{}

void WrappedPluginEditor::resized()
{
    editorComponent.setBounds(getLocalBounds());
}