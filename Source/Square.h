#pragma once
#include <juce_opengl/juce_opengl.h>
#include "JuceHeader.h"

#include <vector>
#include "VertexBufferLayout.h"

struct GridVertex
{
    float x, y, z; 
    float u, v;  
    float zOffset;  
    float r, g, b;
    float pointSize;
};

static_assert(sizeof(GridVertex) == 10 * sizeof(float));

class Square {
public:
    Square(int subdivisions, float size);
    ~Square();

    std::vector<GridVertex> generatePointGrid() const;

    void applyColumnSpectrum(std::vector<GridVertex>& vertices,const std::vector<float>& bandValues, float displacementScale, float pointSize) const;

    static void buildLayout(VertexBufferLayout& layout);

    int getSubdivisions() const { return subdivisions; }
    float getSize() const { return size; }

private:
    int   subdivisions;
    float size;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Square)
};