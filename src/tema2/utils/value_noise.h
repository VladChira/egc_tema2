#pragma once

#include <cmath>       // For sin, floor, etc.
#include <glm/glm.hpp> // For vec2 (optional, or use your own vector implementation)

#include "utils/math_utils.h"

inline float rand(const glm::vec2 &co)
{
    // Matches the GLSL implementation
    return glm::fract(std::sin(glm::dot(co, glm::vec2(12.9898f, 78.233f))) * 43758.5453f);
}

// Smooth interpolation function
inline float smoothstepNoise(float t)
{
    return t * t * (3.0f - 2.0f * t);
}

// Value noise function
inline float valueNoise(const glm::vec2 &uv)
{
    // Floor to get base grid cell
    glm::vec2 i = glm::floor(uv);
    // Fractional part for interpolation
    glm::vec2 f = glm::fract(uv);

    // Get corners' random values
    float v00 = rand(i + glm::vec2(0.0f, 0.0f));
    float v10 = rand(i + glm::vec2(1.0f, 0.0f));
    float v01 = rand(i + glm::vec2(0.0f, 1.0f));
    float v11 = rand(i + glm::vec2(1.0f, 1.0f));

    // Interpolate along x
    float tx = smoothstepNoise(f.x);
    float ty = smoothstepNoise(f.y);

    float nx0 = lerp(v00, v10, tx);
    float nx1 = lerp(v01, v11, tx);

    // Interpolate along y
    return lerp(nx0, nx1, ty);
}
