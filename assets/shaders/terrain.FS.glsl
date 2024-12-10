#version 330 core

in float vHeight;
in vec3 test;
in vec2 uv;
out vec4 fragColor;

///////////////////////////////////////////////
// Helper functions for noise
///////////////////////////////////////////////

// Simple pseudo-random function based on a seed (integer based)
float rand(vec2 co) {
    // A pseudo-random function: This is a cheap hash function
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

// Linear interpolation
float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Smooth interpolation function (ease curve)
float smoothstepNoise(float t) {
    return t * t * (3.0 - 2.0 * t);
}

// Value noise function
float valueNoise(vec2 uv) {
    // Floor to get base grid cell
    vec2 i = floor(uv);
    // Fractional part for interpolation
    vec2 f = fract(uv);

    // Get corners' random values
    float v00 = rand(i + vec2(0.0, 0.0));
    float v10 = rand(i + vec2(1.0, 0.0));
    float v01 = rand(i + vec2(0.0, 1.0));
    float v11 = rand(i + vec2(1.0, 1.0));

    // Interpolate along x
    float tx = smoothstepNoise(f.x);
    float ty = smoothstepNoise(f.y);

    float nx0 = lerp(v00, v10, tx);
    float nx1 = lerp(v01, v11, tx);

    // Interpolate along y
    return lerp(nx0, nx1, ty);
}

void main() {
    // Normalize height to [0,1]
    float normalizedHeight = clamp(vHeight / 17.0f, 0.0, 1.0);

    vec3 fieldGreen = vec3(0.4, 0.9, 0.2);
    vec3 darkGreen = vec3(0.2, 0.4, 0.1);
    vec3 brownGreen = vec3(0.3, 0.4, 0.2);

    vec2 noiseCoords = uv * 10.0f;
    float baseNoise = valueNoise(noiseCoords);
    float detailNoise = valueNoise(noiseCoords * 10.0f);

    vec3 grassColor = mix(fieldGreen, darkGreen, baseNoise);
    grassColor = mix(grassColor, brownGreen, detailNoise * 0.5);

    vec3 color;
    if (normalizedHeight < 0.5) {
        float t = normalizedHeight / 0.5;
        color = mix(fieldGreen, darkGreen, baseNoise);
    } else {
        float t = (normalizedHeight - 0.5) / 0.5;
        color = mix(grassColor, brownGreen, detailNoise * 0.5);
    }

    fragColor = vec4(grassColor, 1.0);
}
