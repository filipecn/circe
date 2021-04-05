#version 440 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout(location = 3) uniform mat4 projection;
layout(location = 4) uniform mat4 model;
layout(location = 5) uniform mat4 view;
layout(location = 6) uniform mat4 lightSpaceMatrix;

// output to fragment shader
out vec3 fPosition;
out vec3 fNormal;
out vec4 fPosLightSpace;

void main() {
    fPosition = vec3(model * vec4(position, 1.0));
    fNormal = mat3(transpose(inverse(model))) * normal;
    fPosLightSpace = lightSpaceMatrix * vec4(fPosition, 1.0);
    gl_Position = projection * view * vec4(fPosition, 1.0);
};



