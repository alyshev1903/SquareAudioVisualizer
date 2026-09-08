#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"

#define FPS 30
#define RELEASE_TIME 0.25f

class Meter : public Component, public Timer
{
public:
    Meter(Colour& c, SquareVisuals& sv, int nv, int nr)
        : meterColor(c), squareVisuals(sv), numVisual(nv), numRaw(nr)
    {
        alpha = exp(-1.0f / (FPS * RELEASE_TIME));
        startTimerHz(FPS);
    }


    ~Meter()
    {}

    void paint(Graphics& g) override
    {

        auto W = getWidth();
        auto H = getHeight();

        g.fillAll(Colours::black);
        g.setColour(Colours::black);
        g.drawRect(0, 0, W, H, 2);

        auto value = squareVisuals[numVisual].getAgent(numRaw).getValue();
        auto meterValue = jmap(value, 0.0f, squareVisuals[numVisual].getAgent(numRaw).getRemapValue(), 0.0f, 1.0f);
        g.setColour(meterColor);
        g.fillRect(1.0f, H - 1.0f - meterValue * getHeight(), W - 2.0f, meterValue * getHeight());

    }

private:

    void timerCallback() override
    {
        repaint();
    }

    float       alpha = 0.0f;
    Colour& meterColor;
    SquareVisuals& squareVisuals;
    int  numVisual;
    int  numRaw;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Meter)

};