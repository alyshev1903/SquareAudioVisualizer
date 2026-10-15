#pragma once

#include <JuceHeader.h>
#include <cmath>
#include "ColorUtils.h"

class MyTheme : public juce::LookAndFeel_V4
{
public:
    MyTheme()
    {
        const juce::Colour ink(0xff1c1c1c);
        const juce::Colour paper(0xffffffff);

        setColour(juce::ResizableWindow::backgroundColourId, paper);

        setColour(juce::Slider::backgroundColourId, paper);
        setColour(juce::Slider::trackColourId, ink);
        setColour(juce::Slider::thumbColourId, ink);
        setColour(juce::Slider::textBoxTextColourId, ink);
        setColour(juce::Slider::textBoxBackgroundColourId, paper);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xffcfcfcf));

        setColour(juce::Label::textColourId, ink);

        setColour(juce::ComboBox::backgroundColourId, paper);
        setColour(juce::ComboBox::textColourId, ink);
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xffcfcfcf));
        setColour(juce::ComboBox::arrowColourId, ink);

        setColour(juce::TextButton::buttonColourId, paper);
        setColour(juce::TextButton::textColourOffId, ink);
        setColour(juce::TextButton::textColourOnId, ink);

        setColour(juce::PopupMenu::backgroundColourId, paper);
        setColour(juce::PopupMenu::textColourId, ink);
        setColour(juce::PopupMenu::highlightedTextColourId, ink);

        setColour(juce::ToggleButton::textColourId, ink);
        setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(0xff9a9a9a));

        setAccentHue(0.0f);
    }

    void setAccentHue(float hue)
    {
        float r, g, b;
        ColorUtils::heatColor(0.35f, r, g, b, hue);
        accent = juce::Colour::fromFloatRGBA(r, g, b, 1.0f).darker(0.15f);

        setColour(juce::ToggleButton::tickColourId, accent);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, accent.withAlpha(0.2f));
    }

    juce::Colour getAccent() const { return accent; }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centre = bounds.getCentre();
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        const int numTicks = 11;
        for (int i = 0; i < numTicks; ++i)
        {
            float t = (float)i / (float)(numTicks - 1);
            float tickAngle = rotaryStartAngle + t * (rotaryEndAngle - rotaryStartAngle);
            bool  lit = t <= sliderPos + 0.001f;

            juce::Point<float> inner(centre.x + (radius * 0.84f) * std::sin(tickAngle), centre.y - (radius * 0.84f) * std::cos(tickAngle));
            juce::Point<float> outer(centre.x + radius * std::sin(tickAngle), centre.y - radius * std::cos(tickAngle));

            g.setColour(lit ? accent : juce::Colour(0xffcfcfcf));
            g.drawLine({ inner, outer }, lit ? 1.8f : 1.2f);
        }

        auto knobRadius = radius * 0.66f;
        juce::Rectangle<float> knobBounds(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

        g.setColour(juce::Colour(0xfff4f4f4));
        g.fillEllipse(knobBounds);
        g.setColour(juce::Colour(0xff9a9a9a));
        g.drawEllipse(knobBounds, 1.2f);

        g.setColour(juce::Colour(0xff1c1c1c));
        g.drawLine(centre.x + (knobRadius * 0.35f) * std::sin(angle), centre.y - (knobRadius * 0.35f) * std::cos(angle),
            centre.x + (knobRadius * 0.9f) * std::sin(angle), centre.y - (knobRadius * 0.9f) * std::cos(angle), 2.0f);
    }

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
        int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox&) override
    {
        auto box = juce::Rectangle<float>(0.0f, 0.0f, (float)width, (float)height).reduced(0.5f);

        g.setColour(juce::Colour(0xfff7f7f7));
        g.fillRoundedRectangle(box, 5.0f);
        g.setColour(juce::Colour(0xffcfcfcf));
        g.drawRoundedRectangle(box, 5.0f, 1.0f);

        juce::Rectangle<float> arrowZone((float)buttonX, (float)buttonY, (float)buttonW, (float)buttonH);
        juce::Path arrow;
        auto arrowCentre = arrowZone.getCentre();
        float arrowSize = 4.0f;
        arrow.startNewSubPath(arrowCentre.x - arrowSize, arrowCentre.y - arrowSize * 0.5f);
        arrow.lineTo(arrowCentre.x, arrowCentre.y + arrowSize * 0.5f);
        arrow.lineTo(arrowCentre.x + arrowSize, arrowCentre.y - arrowSize * 0.5f);
        g.setColour(accent);
        g.strokePath(arrow, juce::PathStrokeType(1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour&,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);

        juce::Colour fill(0xfff7f7f7);
        if (shouldDrawButtonAsDown)
            fill = juce::Colour(0xffe2e2e2);
        else if (shouldDrawButtonAsHighlighted)
            fill = juce::Colour(0xffeeeeee);

        g.setColour(fill);
        g.fillRoundedRectangle(bounds, 5.0f);
        g.setColour(shouldDrawButtonAsHighlighted ? accent : juce::Colour(0xffcfcfcf));
        g.drawRoundedRectangle(bounds, 5.0f, 1.0f);
    }

    juce::Font getComboBoxFont(juce::ComboBox&) override
    {
        return juce::Font(14.0f);
    }

    juce::Font getPopupMenuFont() override
    {
        return juce::Font(14.0f);
    }

private:
    juce::Colour accent = juce::Colours::teal;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyTheme)
};