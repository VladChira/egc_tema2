#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec2 inTexCoord;

out float vHeight;
out vec3 test;
out vec2 uv;

// Uniforms for transformations
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

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
    // Compute noise-based height
    // Use the incoming texture coordinates as a base for noise lookups
    vec2 noiseCoords = (inTexCoord * 10.0f);
    float heightValue = valueNoise(noiseCoords);

    // Scale height
    heightValue *= 20.0f;

    // Modify the vertex position by adding the height to the Y coordinate
    vec3 displacedPosition = inPosition;
    displacedPosition.y += heightValue;

    // Pass the computed height to the fragment shader
    vHeight = displacedPosition.y;

    // Apply standard transformations
    gl_Position = Projection * View * Model * vec4(inPosition, 1.0);

    test = vec3(inTexCoord, 1.0f);
    uv = inTexCoord;
}
