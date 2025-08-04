#pragma once

#include "glm/glm.hpp"

constexpr float inverseLerp(float a, float b, float value) {
    if (a == b) {
        return 0.0f; // or 0.5f to represent midpoint if range is zero
    }
    return (value - a) / (b - a);
}