#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 texCoords;

out vec4 vertexColor;

uniform mat4 proj;
uniform mat4 view;

void main() {
    gl_Position = proj * view * vec4(inPos, 1.0);
    vertexColor = vec4(inColor, 1.0);
}