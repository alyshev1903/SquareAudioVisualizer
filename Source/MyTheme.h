#pragma once

#include <JuceHeader.h>

class MyTheme : public juce::LookAndFeel_V4
{
public:
    MyTheme()
    {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::black);

        setColour(juce::Slider::backgroundColourId, juce::Colours::black);
        setColour(juce::Slider::trackColourId, juce::Colours::white);
        setColour(juce::Slider::thumbColourId, juce::Colours::white);
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::white);

        setColour(juce::Label::textColourId, juce::Colours::white);

        setColour(juce::ComboBox::backgroundColourId, juce::Colours::black);
        setColour(juce::ComboBox::textColourId, juce::Colours::white);
        setColour(juce::ComboBox::outlineColourId, juce::Colours::white);
        setColour(juce::ComboBox::arrowColourId, juce::Colours::white);

        setColour(juce::TextButton::buttonColourId, juce::Colours::black);
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);

        setColour(juce::PopupMenu::backgroundColourId, juce::Colours::black);
        setColour(juce::PopupMenu::textColourId, juce::Colours::white);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colours::white);
        setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::black);

        setColour(juce::ToggleButton::textColourId, juce::Colours::white);
        setColour(juce::ToggleButton::tickColourId, juce::Colours::white);
        setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::grey);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyTheme)
};