#version 430 core

in float vVal;
in vec2 vPos;

uniform float uMinVal = -1.0;
uniform float uMaxVal = 1.0;

out vec4 fCol;

void main() {
    float dist = distance(vPos, vec2(0.5, 0.5));
    if (dist > 0.5) {
        discard;
    }

    vec3 cold = vec3(0.0, 0.2, 1.0);
    vec3 zero = vec3(0.1, 0.1, 0.1);
    vec3 hot = vec3(1.0, 0.2, 0.0);

    vec3 final_col;

    if (vVal < 0.0) {
        // map min, 0.0 -> 0.0, 1.0
        float t = clamp(vVal / uMinVal, 0.0, 1.0);
        final_col = mix(zero, cold, t);
    } else {
        // map 0.0, max -> 0.0, 1.0
        float t = clamp(vVal / uMaxVal, 0.0, 1.0);
        final_col = mix(zero, hot, t);
    }

    fCol = vec4(final_col, 1.0);
}