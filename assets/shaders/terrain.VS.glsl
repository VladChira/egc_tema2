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

void main() {
    // Compute noise-based height
    // Use the incoming texture coordinates as a base for noise lookups
    // vec2 noiseCoords = (inTexCoord * uNoiseFrequency);
    // float heightValue = valueNoise(noiseCoords);

    // // Scale height
    // heightValue *= uTerrainScale;

    // // Modify the vertex position by adding the height to the Y coordinate
    // vec3 displacedPosition = inPosition;
    // displacedPosition.y += heightValue;

    // // Pass the computed height to the fragment shader
    // vHeight = displacedPosition.y;

    // Apply standard transformations
    gl_Position = Projection * View * Model * vec4(inPosition, 1.0);

    test = vec3(inTexCoord, 1.0f);
    uv = inTexCoord;
}
