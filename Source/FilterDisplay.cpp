#include "FilterDisplay.h"

FilterDisplay::FilterDisplay(AudioProcessorValueTreeState& state, SharedAudioData& sharedData)
    : apvts(state), sharedAudioData(sharedData)
{
    startTimerHz(20);
}

FilterDisplay::~FilterDisplay()
{
    stopTimer();
}

void FilterDisplay::setAccent(juce::Colour newAccent)
{
    accent = newAccent;
    repaint();
}

void FilterDisplay::resized()
{
    plotArea = getLocalBounds().toFloat().reduced(8.0f, 6.0f);
    plotArea.removeFromBottom(14.0f);
    numBands = juce::jmax(16, (int)(plotArea.getWidth() / 4.0f));
}

float FilterDisplay::xForFrequency(float hz) const
{
    const float logMin = std::log10((float)Parameters::minFreqAudible);
    const float logMax = std::log10((float)Parameters::maxFreqAudible);
    const float t = (std::log10(juce::jmax(hz, 1.0f)) - logMin) / (logMax - logMin);
    return plotArea.getX() + juce::jlimit(0.0f, 1.0f, t) * plotArea.getWidth();
}

void FilterDisplay::timerCallback()
{
    if (sharedAudioData.readSpectrum(spectrum) <= 0)
        return;

    bands = FFTProcessor::computeBands(spectrum.data(), sharedAudioData.readSampleRate(), numBands,
        (float)Parameters::minFreqAudible, (float)Parameters::maxFreqAudible);
    repaint();
}

void FilterDisplay::paint(juce::Graphics& g)
{
    auto frame = getLocalBounds().toFloat();
    g.setColour(juce::Colours::white);
    g.fillRoundedRectangle(frame, 6.0f);
    g.setColour(juce::Colour(0xffd5d5d5));
    g.drawRoundedRectangle(frame.reduced(0.5f), 6.0f, 1.0f);

    if (plotArea.isEmpty())
        return;

    const float minHz = (float)Parameters::minFreqAudible;
    const float maxHz = (float)Parameters::maxFreqAudible;
    const float floorDb = -48.0f;

    g.setFont(juce::Font(10.0f));

    const float marks[] = { 100.0f, 1000.0f, 10000.0f };
    const char* names[] = { "100", "1k", "10k" };
    for (int i = 0; i < 3; ++i)
    {
        const float x = xForFrequency(marks[i]);
        g.setColour(juce::Colour(0xffe6e6e6));
        g.drawVerticalLine((int)x, plotArea.getY(), plotArea.getBottom());
        g.setColour(juce::Colour(0xff888888));
        g.drawText(names[i], (int)x - 20, (int)plotArea.getBottom() + 1, 40, 12, juce::Justification::centred);
    }

    g.setColour(juce::Colour(0xff888888));
    g.drawText("20 Hz", (int)plotArea.getX(), (int)plotArea.getBottom() + 1, 40, 12, juce::Justification::centredLeft);
    g.drawText("20 kHz", (int)plotArea.getRight() - 40, (int)plotArea.getBottom() + 1, 40, 12, juce::Justification::centredRight);

    const float lowParam = apvts.getRawParameterValue(Parameters::nameFilterLowCut)->load();
    const float highParam = apvts.getRawParameterValue(Parameters::nameFilterHighCut)->load();
    const auto cutoffs = BandFilter::resolveCutoffs(lowParam, highParam, sharedAudioData.readSampleRate());

    if (!bands.empty())
    {
        juce::Path spectrumPath;
        const float bandWidth = plotArea.getWidth() / (float)bands.size();

        spectrumPath.startNewSubPath(plotArea.getX(), plotArea.getBottom());
        for (size_t i = 0; i < bands.size(); ++i)
        {
            const float x = plotArea.getX() + ((float)i + 0.5f) * bandWidth;
            const float y = plotArea.getBottom() - bands[i] * plotArea.getHeight();
            spectrumPath.lineTo(x, y);
        }
        spectrumPath.lineTo(plotArea.getRight(), plotArea.getBottom());
        spectrumPath.closeSubPath();

        g.setColour(accent.withAlpha(0.35f));
        g.fillPath(spectrumPath);
    }

    const float xLow = cutoffs.lowActive ? xForFrequency(cutoffs.low) : plotArea.getX();
    const float xHigh = cutoffs.highActive ? xForFrequency(cutoffs.high) : plotArea.getRight();

    g.setColour(juce::Colour(0xff000000).withAlpha(0.07f));
    g.fillRect(plotArea.getX(), plotArea.getY(), xLow - plotArea.getX(), plotArea.getHeight());
    g.fillRect(xHigh, plotArea.getY(), plotArea.getRight() - xHigh, plotArea.getHeight());

    juce::Path curve;
    const int steps = juce::jmax(2, (int)plotArea.getWidth());
    for (int i = 0; i <= steps; ++i)
    {
        const float t = (float)i / (float)steps;
        const float hz = minHz * std::pow(maxHz / minHz, t);
        const float db = BandFilter::magnitudeDb(hz, cutoffs);
        const float norm = juce::jlimit(0.0f, 1.0f, (db - floorDb) / (0.0f - floorDb));

        const float x = plotArea.getX() + t * plotArea.getWidth();
        const float y = plotArea.getBottom() - norm * (plotArea.getHeight() - 4.0f);

        if (i == 0)
            curve.startNewSubPath(x, y);
        else
            curve.lineTo(x, y);
    }

    g.setColour(juce::Colour(0xff1c1c1c));
    g.strokePath(curve, juce::PathStrokeType(1.6f));
}
