#pragma once

#include <vector>
#include <limits>
#include <cmath>

#include "Data.h"

class Noise
{
public:

    explicit Noise(uint32_t seed) : m_seed(seed) {}

    glm::vec2 randomGradient(int ix, int iy)
    {
        int64_t x = static_cast<int64_t>(ix);
        int64_t y = static_cast<int64_t>(iy);
        uint64_t hash = static_cast<uint64_t>((x * 374761393LL) ^ (y * 668265263LL) ^ m_seed);

        hash ^= (hash >> 13);
        hash *= 1274126177u;
        hash ^= (hash >> 16);

        float angle = (hash % 360) * 3.14159265f / 180.0f;
        return glm::vec2(cos(angle), sin(angle));
    }

    // 1D random gradient
    float randomGradient1D(int ix)
    {
        int64_t x = static_cast<int64_t>(ix);
        uint64_t hash = static_cast<uint64_t>((x * 374761393LL) ^ m_seed);

        hash ^= (hash >> 13);
        hash *= 1274126177u;
        hash ^= (hash >> 16);

        return (hash & 1) ? 1.0f : -1.0f;
    }

    float dotGridGradient(int ix, int iy, float x, float y)
    {
        glm::vec2 gradient = randomGradient(ix, iy);

        float dx = x - (float)ix;
        float dy = y - (float)iy;

        return (dx * gradient.x + dy * gradient.y);
    }

    // 1D dot product
    float dotGridGradient1D(int ix, float x)
    {
        float gradient = randomGradient1D(ix);
        float dx = x - (float)ix;
        return dx * gradient;
    }

    float interpolate(float a0, float a1, float w)
    {
        return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
    }

    float perlin(float x, float y)
    {
        int x0 = (int)x;
        int y0 = (int)y;
        int x1 = x0 + 1;
        int y1 = y0 + 1;

        float sx = x - (float)x0;
        float sy = y - (float)y0;

        float n0 = dotGridGradient(x0, y0, x, y);
        float n1 = dotGridGradient(x1, y0, x, y);
        float ix0 = interpolate(n0, n1, sx);

        n0 = dotGridGradient(x0, y1, x, y);
        n1 = dotGridGradient(x1, y1, x, y);
        float ix1 = interpolate(n0, n1, sx);

        float value = interpolate(ix0, ix1, sy);

        return value;
    }

    // 1D Perlin noise
    float perlin1D(float x)
    {
        int x0 = (int)x;
        int x1 = x0 + 1;

        float sx = x - (float)x0;

        float n0 = dotGridGradient1D(x0, x);
        float n1 = dotGridGradient1D(x1, x);

        float value = interpolate(n0, n1, sx);

        return value;
    }
private:
    uint32_t m_seed;
};