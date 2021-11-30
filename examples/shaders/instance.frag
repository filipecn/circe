#version 440 core

in VERTEX { vec4 color; } vertex;
out vec4 outColor;

void main() {
    outColor = vertex.color;
}
