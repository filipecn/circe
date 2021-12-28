#version 440 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 barycentric;

layout(location = 3) uniform mat4 projection;
layout(location = 4) uniform mat4 model;
layout(location = 5) uniform mat4 view;

out vec3 bc;

void main() {
    bc = barycentric;
    gl_Position = projection * view * model * vec4(position, 1.0);
}
