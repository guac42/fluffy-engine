#version 430 core

layout (location = 0) in vec4 aVert;
layout (location = 1) in vec3 aColor;

uniform mat4 uProjection;

out vec2 texCoord;
out vec3 color;

void main() {
    texCoord = aVert.zw;
    color = aColor;
    gl_Position = uProjection * vec4(aVert.xy, 0.0, 1.0);
}
