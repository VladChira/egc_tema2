#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec2 inTexCoord;

out float vHeight;
out vec3 test;
out vec2 uv;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}


float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float smoothstepNoise(float t) {
    return t * t * (3.0 - 2.0 * t);
}

float valueNoise(vec2 uv) {
    vec2 i = floor(uv);
    vec2 f = fract(uv);

    float v00 = rand(i + vec2(0.0, 0.0));
    float v10 = rand(i + vec2(1.0, 0.0));
    float v01 = rand(i + vec2(0.0, 1.0));
    float v11 = rand(i + vec2(1.0, 1.0));

    float tx = smoothstepNoise(f.x);
    float ty = smoothstepNoise(f.y);

    float nx0 = lerp(v00, v10, tx);
    float nx1 = lerp(v01, v11, tx);

    return lerp(nx0, nx1, ty);
}


void main() {
    vec2 noiseCoords = (inTexCoord * 10.0f);
    float heightValue = valueNoise(noiseCoords);

    heightValue *= 20.0f;

    vec3 displacedPosition = inPosition;
    displacedPosition.y += heightValue;

    vHeight = displacedPosition.y;

    gl_Position = Projection * View * Model * vec4(inPosition, 1.0);

    test = vec3(inTexCoord, 1.0f);
    uv = inTexCoord;
}
