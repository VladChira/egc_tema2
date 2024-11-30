#pragma once
#include <cmath>
#include "utils/math_utils.h"

inline static float hash(int x) {
    x = (x << 13) ^ x;
    return 1.0f - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f;
}

inline float value_noise(float x) {
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    float t = x - x0;
    float fade_t = t * t * (3 - 2 * t);

    float value0 = hash(x0);
    float value1 = hash(x1);

    return lerp(value0, value1, fade_t);
}