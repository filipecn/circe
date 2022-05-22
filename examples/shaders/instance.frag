#version 440 core

in VERTEX {
    vec4 color;
    vec2 uv;
} vertex;

out vec4 outColor;

void main() {
    outColor = vec4(vertex.uv, 0, 1);
}
