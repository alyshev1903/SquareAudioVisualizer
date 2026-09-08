#include "PluginProcessor.h"
#include "PluginEditor.h"

SquareAudioVisualizerAudioProcessorEditor::SquareAudioVisualizerAudioProcessorEditor(SquareAudioVisualizerAudioProcessor& p, AudioProcessorValueTreeState& vts, SquareVisuals& sv)
    : audioProcessor(p), valueTreeState(vts), squareVisuals(sv),
    m_GLComponent(sv[0], p.getSharedAudioData(), vts)
{
    setLookAndFeel(&Theme);

    auto setupSlider = [this](juce::Slider& slider, juce::Label& label, const juce::String& text)
    {
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        addAndMakeVisible(slider);

        label.setText(text, juce::dontSendNotification);
        label.attachToComponent(&slider, true);
        addAndMakeVisible(label);
    };

    setupSlider(subdivisionsSlider, subdivisionsLabel, "Subdivisions");
    setupSlider(sizeSlider, sizeLabel, "Size");
    setupSlider(pointSizeSlider, pointSizeLabel, "Point size");
    setupSlider(inputLevelSlider, inputLevelLabel, "Input Level");
    inputLevelSlider.setTextValueSuffix(" dB");

    pointOrTextureBox.addItemList(Parameters::PointOrTextureOptions, 1);
    addAndMakeVisible(pointOrTextureBox);

    addAndMakeVisible(loadTextureButton);
    loadTextureButton.onClick = [this] { loadTextureButtonClicked(); };

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

void SquareAudioVisualizerAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto controlsArea = bounds.removeFromBottom(164).reduced(8);
    m_GLComponent.setBounds(bounds);

    const int rowHeight = 24;
    const int labelWidth = 90;

    auto layoutRow = [&](juce::Slider& slider)
    {
        auto row = controlsArea.removeFromTop(rowHeight);
        row.removeFromLeft(labelWidth);
        slider.setBounds(row.reduced(2));
    };

    layoutRow(subdivisionsSlider);
    layoutRow(sizeSlider);
    layoutRow(pointSizeSlider);
    layoutRow(inputLevelSlider);

    controlsArea.removeFromTop(16);

    auto bottomRow = controlsArea.removeFromTop(rowHeight);
    pointOrTextureBox.setBounds(bottomRow.removeFromLeft(180));
    bottomRow.removeFromLeft(12);
    loadTextureButton.setBounds(bottomRow.removeFromLeft(180));
    bottomRow.removeFromLeft(20);
    zcrToggle.setBounds(bottomRow.removeFromLeft(90));
    bottomRow.removeFromLeft(8);
    pitchToggle.setBounds(bottomRow.removeFromLeft(90));
    bottomRow.removeFromLeft(20);
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
    new OpenGLWindow(audioProcessor, valueTreeState, squareVisuals);
}

void SquareAudioVisualizerAudioProcessorEditor::comboBoxChanged(juce::ComboBox* box)
{}

void SquareAudioVisualizerAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{}

WrappedPluginEditor::WrappedPluginEditor(SquareAudioVisualizerAudioProcessor& p, AudioProcessorValueTreeState& vts, SquareVisuals& s)
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