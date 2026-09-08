#pragma once
#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <vector>

#include "../Source/Parameters.h"
#include "../Source/FFTProcessor.h"

class SharedAudioData
{
public:
    static constexpr int maxAgents = MAX_NUM_VISUALS;
    static constexpr int maxHarmonics = 6;
    static constexpr int spectrumBins = FFTProcessor::fftSize / 2;

    void writeAgentValue(int agentIndex, float value)
    {
        jassert(agentIndex < maxAgents);
        agentValues[agentIndex].store(value);
    }

    float readAgentValue(int agentIndex) const
    {
        jassert(agentIndex < maxAgents);
        return agentValues[agentIndex].load();
    }

    void writePitch(float frequencyHz)
    {
        dominantFrequency.store(frequencyHz);
    }

    float readPitch() const
    {
        return dominantFrequency.load();
    }

    void writeHarmonics(const std::vector<float>& harmonicFrequencies)
    {
        int count = juce::jmin((int)harmonicFrequencies.size(), maxHarmonics);
        harmonicCount.store(count);
        for (int i = 0; i < count; ++i)
            harmonics[i].store(harmonicFrequencies[i]);
    }

    int readHarmonicCount() const
    {
        return harmonicCount.load();
    }

    float readHarmonic(int index) const
    {
        jassert(index < maxHarmonics);
        return harmonics[index].load();
    }

    std::vector<float> readAllHarmonics() const
    {
        int count = harmonicCount.load();
        std::vector<float> result(count);
        for (int i = 0; i < count; ++i)
            result[i] = harmonics[i].load();
        return result;
    }

    void writeSpectrum(const float* magnitudes, int count)
    {
        int n = juce::jmin(count, spectrumBins);
        for (int i = 0; i < n; ++i)
            spectrum[i].store(magnitudes[i]);
        spectrumBinsWritten.store(n);
    }

    int readSpectrum(std::vector<float>& outMagnitudes) const
    {
        int n = spectrumBinsWritten.load();
        outMagnitudes.resize((size_t)n);
        for (int i = 0; i < n; ++i)
            outMagnitudes[(size_t)i] = spectrum[i].load();
        return n;
    }

    void writeSampleRate(double sr) { sampleRate.store(sr); }
    double readSampleRate() const { return sampleRate.load(); }

private:
    std::array<std::atomic<float>, maxAgents> agentValues{ 0.0f };
    std::atomic<float> dominantFrequency{ 0.0f };
    std::array<std::atomic<float>, maxHarmonics> harmonics{ 0.0f };
    std::atomic<int> harmonicCount{ 0 };

    std::array<std::atomic<float>, spectrumBins> spectrum{};
    std::atomic<int> spectrumBinsWritten{ 0 };
    std::atomic<double> sampleRate{ 44100.0 };
};