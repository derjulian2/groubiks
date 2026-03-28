#version 330 core

in vec4 vertexColor;
in vec2 vertexTexCoords;

out vec4 fragColor;

uniform sampler2D texture0;

void main() {
    fragColor = texture(texture0, vertexTexCoords) * vertexColor;
}