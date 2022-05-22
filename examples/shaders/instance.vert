#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uvs;
layout (location = 2) in vec4 color;
layout (location = 3) in mat4 transform_matrix;

layout (location = 4) uniform mat4 model_view_matrix;
layout (location = 5) uniform mat4 projection_matrix;

out VERTEX {
    vec4 color;
    vec2 uv;
} vertex;

void main() {
    gl_Position = projection_matrix * model_view_matrix * transform_matrix * vec4(position, 1);
    vertex.color = color;
    vertex.uv = uvs;
}
