#version 440 core
out vec4 FragColor;

in vec2 fuv;

uniform sampler2D depthMap;

void main()
{
    float depthValue = texture(depthMap, fuv).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}