#version 440 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec3 fNormal;
out vec3 fPosition;
out vec2 fUV;
out vec4 fPosLightSpace;

layout(location = 11) uniform mat4 model;
layout(location = 12) uniform mat4 view;
layout(location = 13) uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main() {
    fPosition = vec3(model * vec4(position, 1.0));
    fNormal = mat3(transpose(inverse(model))) * normal;
    fUV = texcoord;
    fPosLightSpace = lightSpaceMatrix * vec4(fPosition, 1.0);
    gl_Position = projection * view * vec4(fPosition, 1.0);
}






