#version 330 core

in float vHeight;
in vec3 test;
in vec2 uv;
out vec4 fragColor;

uniform float droneHeight;

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
    float normalizedHeight = clamp(vHeight / 17.0f, 0.0, 1.0);

    vec3 fieldGreen = vec3(0.4, 0.9, 0.2);
    vec3 darkGreen = vec3(0.2, 0.4, 0.1);
    vec3 brownGreen = vec3(0.3, 0.4, 0.2);
    vec3 c1 = vec3(0.8, 0.1, 0.1);
    vec3 c2 = vec3(0.1, 0.1, 0.1);

    vec2 noiseCoords = uv * 10.0f;
    float baseNoise = valueNoise(noiseCoords);
    float detailNoise = valueNoise(noiseCoords * 10.0f);

    vec3 grassColor = mix(fieldGreen, darkGreen, baseNoise);
    grassColor = mix(grassColor, brownGreen, detailNoise * 0.5);

    vec3 color;
    if (droneHeight < 80.0) {
        float t = droneHeight / 80.0;
        color = mix(grassColor, brownGreen, t);
    } else {
        float t = (droneHeight - 80.0) / 80.0;
        color = mix(c1, c2, t);
    }

    fragColor = vec4(color, 1.0);
}
