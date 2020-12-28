#version 440 core
// regular vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
//layout(location = 2) in vec2 uv;

// per instance attributes
layout(location = 3) uniform mat4 projection;
layout(location = 4) uniform mat4 model;
layout(location = 5) uniform mat4 view;

// output to fragment shader
out vec3 fPosition;
out vec3 fNormal;

void main() {
    fPosition = vec3(model * vec4(position, 1.0));
    fNormal = mat3(transpose(inverse(model))) * normal;
    gl_Position = projection * view * vec4(fPosition, 1.0);
};



