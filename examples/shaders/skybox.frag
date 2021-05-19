#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
//    vec3 envColor = texture(skybox, TexCoords).rgb;
    vec3 envColor = textureLod(skybox, TexCoords, 1.2).rgb;

    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));

    FragColor = vec4(envColor, 1.0);
}