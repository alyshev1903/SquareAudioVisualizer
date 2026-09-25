#pragma once
#include <JuceHeader.h>

namespace ColorUtils
{
    inline void heatColor(float t, float& r, float& g, float& b, float hueShift = 0.0f)
    {
        t = juce::jlimit(0.0f, 1.0f, t);

        struct ColorStop { float r, g, b; };
        static const ColorStop stops[] = {
            { 0.05f, 0.05f, 0.30f },
            { 0.10f, 0.55f, 0.90f },
            { 0.15f, 0.85f, 0.25f },
            { 0.95f, 0.85f, 0.10f },
            { 0.95f, 0.15f, 0.10f }
        };
        constexpr int numStops = 5;

        float scaled = t * (float)(numStops - 1);
        int   i0 = juce::jlimit(0, numStops - 2, (int)scaled);
        int   i1 = i0 + 1;
        float frac = scaled - (float)i0;

        r = stops[i0].r * (1.0f - frac) + stops[i1].r * frac;
        g = stops[i0].g * (1.0f - frac) + stops[i1].g * frac;
        b = stops[i0].b * (1.0f - frac) + stops[i1].b * frac;

        if (hueShift > 0.0f)
        {
            juce::Colour shifted = juce::Colour::fromFloatRGBA(r, g, b, 1.0f).withRotatedHue(hueShift);
            r = shifted.getFloatRed();
            g = shifted.getFloatGreen();
            b = shifted.getFloatBlue();
        }
    }
}