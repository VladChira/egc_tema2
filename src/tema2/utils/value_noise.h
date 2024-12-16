#pragma once

#include <cmath>
#include <glm/glm.hpp>

#include "utils/math_utils.h"

inline float rand(const glm::vec2 &co)
{
    return glm::fract(std::sin(glm::dot(co, glm::vec2(12.9898f, 78.233f))) * 43758.5453f);
}

inline float smoothstepNoise(float t)
{
    return t * t * (3.0f - 2.0f * t);
}

inline float valueNoise(const glm::vec2 &uv)
{
    glm::vec2 i = glm::floor(uv);
    glm::vec2 f = glm::fract(uv);

    float v00 = rand(i + glm::vec2(0.0f, 0.0f));
    float v10 = rand(i + glm::vec2(1.0f, 0.0f));
    float v01 = rand(i + glm::vec2(0.0f, 1.0f));
    float v11 = rand(i + glm::vec2(1.0f, 1.0f));

    float tx = smoothstepNoise(f.x);
    float ty = smoothstepNoise(f.y);

    float nx0 = lerp(v00, v10, tx);
    float nx1 = lerp(v01, v11, tx);
    return lerp(nx0, nx1, ty);
}
