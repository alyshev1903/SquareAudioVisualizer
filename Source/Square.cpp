#include "Square.h"
#include "ColorUtils.h"

using namespace ::juce::gl;

namespace
{
    constexpr float kDefaultPointSize = 3.0f;
}

Square::Square(int subdivisionsIn, float sizeIn)
    : subdivisions(juce::jmax(1, subdivisionsIn)), size(sizeIn)
{}

Square::~Square() {}

std::vector<GridVertex> Square::generatePointGrid() const
{
    std::vector<GridVertex> vertices;
    vertices.reserve((size_t)(subdivisions + 1) * (size_t)(subdivisions + 1));

    const float step = size / (float)subdivisions;

    for (int i = 0; i <= subdivisions; ++i)
    {
        for (int j = 0; j <= subdivisions; ++j)
        {
            GridVertex v{};
            v.x = -size / 2.0f + (float)j * step;
            v.y = -size / 2.0f + (float)i * step;
            v.z = 0.0f;
            v.u = (float)j / (float)subdivisions;
            v.v = (float)i / (float)subdivisions;
            v.zOffset = 0.0f;
            v.r = v.g = v.b = 1.0f;
            v.pointSize = kDefaultPointSize;
            vertices.push_back(v);
        }
    }
    return vertices;
}

void Square::applyColumnSpectrum(std::vector<GridVertex>& vertices, const std::vector<float>& bandValues, float displacementScale, float pointSize, float hueShift) const
{
    if (bandValues.empty())
        return;

    const int cols = subdivisions + 1;
    const int rows = subdivisions + 1;

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            int bandIndex = (int)(((float)j / (float)juce::jmax(1, cols - 1)) * (float)(bandValues.size() - 1));
            bandIndex = juce::jlimit(0, (int)bandValues.size() - 1, bandIndex);
            float value = bandValues[(size_t)bandIndex];
            GridVertex& vert = vertices[(size_t)(i * cols + j)];
            vert.zOffset = value * displacementScale;
            vert.pointSize = pointSize;
            ColorUtils::heatColor(value, vert.r, vert.g, vert.b, hueShift);
        }
    }
}



void Square::buildLayout(VertexBufferLayout& layout)
{
    layout.Push<float>(3);
    layout.Push<float>(2);
    layout.Push<float>(1);
    layout.Push<float>(3);
    layout.Push<float>(1);
}