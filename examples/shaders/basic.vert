#version 440 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 fNormal;
out vec3 fPosition;
out vec4 fPosLightSpace;

//layout(std140) uniform Transforms {
layout (location = 0) uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;
//};

void main() {
    fPosition = vec3(model * vec4(position, 1.0));
    fNormal = mat3(transpose(inverse(model))) * normal;
    fPosLightSpace = lightSpaceMatrix * vec4(fPosition, 1.0);
    gl_Position = projection * view * vec4(fPosition, 1.0);
}