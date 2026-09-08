#pragma once

#include "FeaturesExtractor.h"

#include "Envelope.h"
#include "Pitch.h"
#include "ZCR.h"

class FeatureCreator
{
public:
    FeatureCreator() {};
    ~FeatureCreator() {};

    std::unique_ptr<FeaturesExtractor> createFeature(FeatureType type)
    {
        switch (type)
        {
        case FeatureType::ENVELOPE: return std::make_unique<Envelope>(type);
        case FeatureType::ZCR:      return std::make_unique<ZCR>(type);
        case FeatureType::PITCH:    return std::make_unique<Pitch>(type);
        default:
            return nullptr;
        }
    }
private:


};