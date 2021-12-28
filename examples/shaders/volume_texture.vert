#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 uvw;

layout (location = 2) uniform mat4 model_view_matrix;
layout (location = 3) uniform mat4 projection_matrix;

out vec3 tex;

void main() {
    gl_Position = projection_matrix * model_view_matrix *
    vec4(position, 1.0);
    tex = uvw;
}
