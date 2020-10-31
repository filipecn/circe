#version 440 core
layout(location = 0) out vec4 fragColor;

in vec3 fNormal;
in vec3 fPosition;
in vec2 fUV;
in vec4 fPosLightSpace;

struct Light {
    vec3 direction;// directional light direction
    vec3 point;// point light position
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

layout(location = 1) uniform Light light;
layout(location = 6) uniform Material material;
layout(location = 10) uniform vec3 cameraPosition;
layout(location = 14) uniform vec2 screenResolution;

uniform sampler2D shadowMap;

float computeShadow(vec4 fPosLS) {
    // perform perspective divide
    vec3 projCoords = fPosLS.xyz / fPosLS.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0)
    return 0.0;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    //    float bias = max(0.05 * (1.0 - dot(fNormal, light.direction)), 0.005);
    //    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

// http://iquilezles.org/www/articles/filterableprocedurals/filterableprocedurals.htm
const float N = 10.0;// grid ratio
float gridTextureGradBox(in vec2 p, in vec2 ddx, in vec2 ddy)
{
    // filter kernel
    vec2 w = max(abs(ddx), abs(ddy)) + 0.01;

    // analytic (box) filtering
    vec2 a = p + 0.5*w;
    vec2 b = p - 0.5*w;
    vec2 i = (floor(a)+min(fract(a)*N, 1.0)-
    floor(b)-min(fract(b)*N, 1.0))/(N*w);
    //pattern
    return (1.0-i.x)*(1.0-i.y);
}

void main() {
    vec2 ddx_uvw = dFdx(fUV);
    vec2 ddy_uvw = dFdy(fUV);
    vec3 col = vec3(1.0)*gridTextureGradBox(fUV * 100, ddx_uvw, ddy_uvw);


    vec3 N = normalize(fNormal);
    vec3 V = normalize(cameraPosition - fPosition);
    vec2 uv = fUV;
    float size = 1.0/100.0;// size of the tile
    float edge = size/32.0;// size of the edge
    uv = (mod(uv, size) - mod(uv - edge, size) - edge) * 1.0/size;
    // FINAL COLOR
    fragColor = vec4((0.9 - length(uv) * 0.5) * dot(N, V));

    vec2 p = (-screenResolution.xy + 2.0*gl_FragCoord.xy) / screenResolution.y;
    // fog
    float t = length(cameraPosition - fPosition);
    col = mix(col, vec3(0.7), 1.0-exp(-0.001*t*t));
    // calculate shadow
    float shadow = computeShadow(fPosLightSpace);
    col *= 1.0 - shadow;
    // gamma correction
    col = pow(col, vec3(0.4545));
    fragColor = vec4(col, 1.0);
}