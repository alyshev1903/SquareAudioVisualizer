#pragma once

#include <JuceHeader.h>
#include "SquareVisual.h"
#include "SharedAudioData.h"

typedef std::array <SquareVisual, MAX_NUM_VISUALS > SquareVisuals;

class SquareAudioVisualizerAudioProcessor : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    SquareAudioVisualizerAudioProcessor();
    ~SquareAudioVisualizerAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;

    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    bool getWindowOpened();

    void setWindowOpened(bool value);

    SharedAudioData& getSharedAudioData() { return sharedAudioData; }

private:
    AudioProcessorValueTreeState parameters;
	SharedAudioData sharedAudioData;
    SquareVisuals squareVisuals;

    void parameterChanged(const String& paramID, float newValue) override;

	bool windowOpened = DEFAULT_WINDOW_OPENED;
     
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquareAudioVisualizerAudioProcessor)
};