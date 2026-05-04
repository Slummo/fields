#version 430 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in float iVal;

uniform int uGridWidth;
uniform float uCellSize;
uniform mat4 uView;
uniform mat4 uProj;

out float vVal;
out vec2 vPos;

void main() {
    int x = gl_InstanceID % uGridWidth;
    int y = gl_InstanceID / uGridWidth;

    vec2 final = aPos * uCellSize;
    final += vec2(x * uCellSize, y * uCellSize);

    vVal = iVal;
    gl_Position = uProj * uView * vec4(final, 0.0, 1.0);
}