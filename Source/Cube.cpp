#include "Cube.h"
#include "ColorUtils.h"
#include <cmath>

Cube::Cube(int subdivisionsIn, float sizeIn)
    : subdivisions(juce::jmax(1, subdivisionsIn)), size(sizeIn)
{
    const int n = subdivisions + 1;
    basePositions.reserve((size_t)n * (size_t)n * (size_t)n);

    const float step = size / (float)subdivisions;
    const float half = size / 2.0f;

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            for (int k = 0; k < n; ++k)
                basePositions.push_back(glm::vec3(-half + (float)i * step, -half + (float)j * step, -half + (float)k * step));
}

Cube::~Cube() {}

std::vector<GridVertex> Cube::buildVertexData(const std::vector<float>& bands,float displacementScale, float pointSize, float hueShift) const
{
    std::vector<GridVertex> vertices;
    vertices.reserve(basePositions.size());

    if (bands.empty())
    {
        for (const auto& p : basePositions)
        {
            GridVertex v{};
            v.x = p.x; v.y = p.y; v.z = p.z;
            v.r = v.g = v.b = 1.0f;
            v.pointSize = pointSize;
            vertices.push_back(v);
        }
        return vertices;
    }

    const float half = size / 2.0f;
    const int   numBands = (int)bands.size();

    auto bandIndexFor = [&](float coord) -> int
    {
        float t = (coord + half) / size;
        int idx = (int)(t * (float)(numBands - 1));
        return juce::jlimit(0, numBands - 1, idx);
    };

    for (const auto& p : basePositions)
    {
        float ax = std::abs(p.x), ay = std::abs(p.y), az = std::abs(p.z);

        GridVertex v{};
        float bandValue;

        if (ax >= ay && ax >= az)
        {
            bandValue = bands[(size_t)bandIndexFor(p.y)];
            float sign = p.x >= 0.0f ? 1.0f : -1.0f;
            v.x = p.x + sign * bandValue * displacementScale;
            v.y = p.y;
            v.z = p.z;
        }
        else if (ay >= az)
        {
            bandValue = bands[(size_t)bandIndexFor(p.z)];
            float sign = p.y >= 0.0f ? 1.0f : -1.0f;
            v.x = p.x;
            v.y = p.y + sign * bandValue * displacementScale;
            v.z = p.z;
        }
        else
        {
            bandValue = bands[(size_t)bandIndexFor(p.x)];
            float sign = p.z >= 0.0f ? 1.0f : -1.0f;
            v.x = p.x;
            v.y = p.y;
            v.z = p.z + sign * bandValue * displacementScale;
        }

        ColorUtils::heatColor(bandValue, v.r, v.g, v.b, hueShift);
        v.pointSize = pointSize;
        vertices.push_back(v);
    }

    return vertices;
}