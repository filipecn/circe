#version 440 core

in vec3 tex;
layout(location = 0) out vec4 fragColor;

layout(location = 4) uniform vec3 cameraPosition;
layout(location = 5) uniform sampler3D g_densityTex;
layout(location = 6) uniform vec3 g_lightPos;
layout(location = 7) uniform vec3 g_lightIntensity;
layout(location = 8) uniform float g_absorption;


void main() {
    // diagonal of the cube
    const float maxDist = sqrt(3.0);
    const int numSamples = 128;
    const float scale = maxDist/float(numSamples);
    const int numLightSamples = 32;
    const float lscale = maxDist / float(numLightSamples);
    // assume all coordinates are in texture space
    vec3 pos = tex;
    vec3 eyeDir = normalize(pos-cameraPosition)*scale;
    // transmittance
    float T = 1.0;
    // in-scattered radiance
    vec3 Lo = vec3(0.0);
    for (int i=0; i < numSamples; ++i) {
        // sample density
        float density = texture(g_densityTex, pos).x;
        if (pos.x < 0.0 || pos.y < 0.0 || pos.z < 0.0 || pos.x > 1.0 ||
        pos.y > 1.0 || pos.z > 1.0) density = 0.;
        // skip empty space
        if (density > 0.0) {
            // attenuate ray-throughput
            T *= 1.0-density*scale*g_absorption;
            if (T <= 0.01)
            break;
            // point light dir in texture space
            vec3 lightDir = normalize(g_lightPos-pos)*lscale;
            // sample light
            // transmittance along light ray
            float Tl = 1.0;
            vec3 lpos = pos + lightDir;
            for (int s=0; s < numLightSamples; ++s) {
                float ld = texture(g_densityTex, lpos).x;
                if (lpos.x < 0.0 || lpos.y < 0.0 || lpos.z < 0.0 || lpos.x > 1.0 ||
                lpos.y > 1.0 || lpos.z > 1.0) ld = 0.;
                Tl *= 1.0-g_absorption*lscale*ld;
                if (Tl <= 0.01)
                break;
                lpos += lightDir;
            }
            vec3 Li = g_lightIntensity*Tl;
            Lo += Li*T*density*scale;
        }
        pos += eyeDir;
    }
    fragColor.xyz = Lo;
    fragColor.w = 1.0 - T;
}