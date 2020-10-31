#version 440 core
layout(location = 0) out vec4 fragColor;

in vec3 fNormal;
in vec3 fPosition;
in vec4 fPosLightSpace;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 kAmbient;
    vec3 kDiffuse;
    vec3 kSpecular;
    float shininess;
};

layout (std140) uniform Scene {
    Light light;
};


uniform Material material;
layout(location = 10) uniform vec3 cameraPosition;
uniform sampler2D shadowMap;

float computeShadow(vec4 fPosLS) {
    // perform perspective divide
    vec3 projCoords = fPosLS.xyz / fPosLS.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

void main() {
    vec3 norm = normalize(fNormal);
    vec3 lightDirection = normalize(light.position.xyz - fPosition);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 viewDir = normalize(cameraPosition - fPosition);
    vec3 halfwayDirection = normalize(lightDirection + viewDir);
    // vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, halfwayDirection), 0.0), material.shininess);
    // calculate shadow
    float shadow = computeShadow(fPosLightSpace);

    vec3 lightIntensity = (light.ambient * material.kAmbient +
    (1.0 - shadow)*
    (light.diffuse * material.kDiffuse * diff +
    light.specular * material.kSpecular * spec)) * vec3(1, 1, 1);
    fragColor = vec4(lightIntensity, 1.0);
}