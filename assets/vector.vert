#version 430 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 iVal;

uniform int uGridWidth;
uniform float uCellSize;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 vCol;

void main() {
    int x = gl_InstanceID % uGridWidth;
    int y = gl_InstanceID / uGridWidth;

    float mag = length(iVal);
    if (mag < 0.0001) {
        gl_Position = vec4(0.0);
        return;
    }

    vec2 dir = iVal / mag;
    mat2 rot = mat2(dir.x, dir.y, -dir.y, dir.x);

    vec2 final = aPos;
    final *= min(mag, 1.0);    // scale by strength
    final *= uCellSize * 0.7;  // scale to cell size
    final = rot * final;       // point in the flow direction

    // Move to the center of the grid cell
    final += vec2(x * uCellSize + (uCellSize / 2.0), y * uCellSize + (uCellSize / 2.0));

    vCol = vec3(0.0, 0.8, 1.0);
    gl_Position = uProj * uView * vec4(final, 0.0, 1.0);
}