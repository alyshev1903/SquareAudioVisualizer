#pragma once

#include <JuceHeader.h>
#include <cmath>

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

    void setAccentHue(float hue) { accentHue = hue; }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centre = bounds.getCentre();
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        juce::Colour accent = juce::Colours::white.withRotatedHue(accentHue);

        g.setColour(juce::Colours::black.withAlpha(0.6f));
        g.fillEllipse(centre.x - radius * 0.62f + 2.0f, centre.y - radius * 0.62f + 3.0f, radius * 1.24f, radius * 1.24f);

        const int numTicks = 11;
        for (int i = 0; i < numTicks; ++i)
        {
            float t = (float)i / (float)(numTicks - 1);
            float tickAngle = rotaryStartAngle + t * (rotaryEndAngle - rotaryStartAngle);
            bool  lit = t <= sliderPos + 0.001f;

            juce::Point<float> inner(centre.x + (radius * 0.88f) * std::sin(tickAngle), centre.y - (radius * 0.88f) * std::cos(tickAngle));
            juce::Point<float> outer(centre.x + radius * std::sin(tickAngle), centre.y - radius * std::cos(tickAngle));

            g.setColour(lit ? accent : juce::Colours::white.withAlpha(0.25f));
            g.drawLine({ inner, outer }, lit ? 2.0f : 1.2f);
        }

        auto knobRadius = radius * 0.62f;
        juce::Rectangle<float> knobBounds(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

        juce::ColourGradient bodyGradient(juce::Colour(0xff3a3a3a), centre.x - knobRadius * 0.6f, centre.y - knobRadius * 0.6f,
            juce::Colour(0xff0a0a0a), centre.x + knobRadius * 0.6f, centre.y + knobRadius * 0.6f, true);
        g.setGradientFill(bodyGradient);
        g.fillEllipse(knobBounds);

        g.setColour(juce::Colours::white.withAlpha(0.35f));
        g.drawEllipse(knobBounds, 1.5f);

        auto glossRadius = knobRadius * 0.55f;
        juce::Point<float> glossCentre(centre.x - knobRadius * 0.35f, centre.y - knobRadius * 0.4f);
        juce::ColourGradient gloss(juce::Colours::white.withAlpha(0.25f), glossCentre.x, glossCentre.y,
            juce::Colours::white.withAlpha(0.0f), glossCentre.x, glossCentre.y + glossRadius, true);
        g.setGradientFill(gloss);
        g.fillEllipse(glossCentre.x - glossRadius, glossCentre.y - glossRadius, glossRadius * 2.0f, glossRadius * 2.0f);

        juce::Path pointer;
        auto pointerLength = knobRadius * 0.85f;
        pointer.startNewSubPath(centre.x + (knobRadius * 0.2f) * std::sin(angle), centre.y - (knobRadius * 0.2f) * std::cos(angle));
        pointer.lineTo(centre.x + pointerLength * std::sin(angle), centre.y - pointerLength * std::cos(angle));
        g.setColour(accent);
        g.strokePath(pointer, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        g.setColour(accent);
        g.fillEllipse(centre.x - 2.5f, centre.y - 2.5f, 5.0f, 5.0f);
    }

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
        int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox&) override
    {
        auto outer = juce::Rectangle<float>(0.0f, 0.0f, (float)width, (float)height).reduced(1.0f);
        auto cornerSize = 6.0f;

        juce::Colour accent = juce::Colours::white.withRotatedHue(accentHue);

        juce::ColourGradient bodyGradient(juce::Colour(0xff3a3a3a), outer.getX(), outer.getY(),
            juce::Colour(0xff0a0a0a), outer.getRight(), outer.getBottom(), false);
        g.setGradientFill(bodyGradient);
        g.fillRoundedRectangle(outer, cornerSize);

        auto glossArea = outer.withHeight(outer.getHeight() * 0.4f).reduced(2.0f);
        g.setColour(juce::Colours::white.withAlpha(0.08f));
        g.fillRoundedRectangle(glossArea, cornerSize * 0.7f);

        g.setColour(juce::Colours::white.withAlpha(0.35f));
        g.drawRoundedRectangle(outer, cornerSize, 1.2f);

        juce::Rectangle<float> arrowZone((float)buttonX, (float)buttonY, (float)buttonW, (float)buttonH);
        juce::Path arrow;
        auto arrowCentre = arrowZone.getCentre();
        float arrowSize = 5.0f;
        arrow.startNewSubPath(arrowCentre.x - arrowSize, arrowCentre.y - arrowSize * 0.5f);
        arrow.lineTo(arrowCentre.x, arrowCentre.y + arrowSize * 0.5f);
        arrow.lineTo(arrowCentre.x + arrowSize, arrowCentre.y - arrowSize * 0.5f);
        g.setColour(accent);
        g.strokePath(arrow, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour&,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
        auto cornerSize = 6.0f;

        juce::Colour accent = juce::Colours::white.withRotatedHue(accentHue);

        juce::Colour top = juce::Colour(0xff3a3a3a);
        juce::Colour bottom = juce::Colour(0xff0a0a0a);
        if (shouldDrawButtonAsDown)
        {
            top = juce::Colour(0xff1a1a1a);
            bottom = juce::Colour(0xff050505);
        }

        juce::ColourGradient bodyGradient(top, bounds.getX(), bounds.getY(),
            bottom, bounds.getRight(), bounds.getBottom(), false);
        g.setGradientFill(bodyGradient);
        g.fillRoundedRectangle(bounds, cornerSize);

        auto glossArea = bounds.withHeight(bounds.getHeight() * 0.4f).reduced(2.0f);
        g.setColour(juce::Colours::white.withAlpha(shouldDrawButtonAsDown ? 0.03f : 0.08f));
        g.fillRoundedRectangle(glossArea, cornerSize * 0.7f);

        g.setColour((shouldDrawButtonAsHighlighted ? accent : juce::Colours::white).withAlpha(0.35f));
        g.drawRoundedRectangle(bounds, cornerSize, 1.2f);
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
    float accentHue = 0.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyTheme)
};