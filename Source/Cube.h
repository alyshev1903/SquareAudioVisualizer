#pragma once
#include <JuceHeader.h>
#include <vector>
#include "vendor/glm/glm.hpp"
#include "Square.h"

class Cube
{
public:
    Cube(int subdivisions, float size);
    ~Cube();

    std::vector<GridVertex> buildVertexData(const std::vector<float>& bands, float displacementScale, float pointSize, float hueShift) const;

private:
    int subdivisions;
    float size;
    std::vector<glm::vec3> basePositions;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Cube)
};