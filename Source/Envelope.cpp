#include "Envelope.h"

Envelope::Envelope(FeatureType type)
    : FeaturesExtractor(type)
{}
Envelope::~Envelope() {}


void Envelope::processSample(float sample)
{
	envelope = jmax(0.0f, smoothingFactor * envelope + (1.0f - smoothingFactor) * std::abs(sample));
}
float Envelope::getValue()
{
    return envelope;
}
void  Envelope::setSampleRate(double sampleRate) {};
void  Envelope::setFFTSize(int newFFTsize) {};
void  Envelope::setFFT(float* newData) {};
void  Envelope::computeValueFromFFTData() {};

String Envelope::getName()
{
    return "Envelope";
}