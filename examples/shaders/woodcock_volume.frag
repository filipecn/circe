#version 440 core

#define M_PI 3.1415926535897932384626433832795

in vec3 tex;
in vec3 fPosition;

layout(location = 0) out vec4 fragColor;
layout(location = 5) uniform sampler3D extinction_tex;
layout(location = 9) uniform vec3 domain_box_lower;
layout(location = 11) uniform vec3 domain_box_upper;
layout(location = 12) uniform float max_extinction;
layout(location = 13) uniform int max_iterations;
layout(location = 14) uniform int max_interactions;
layout(location = 15) uniform float albedo;

struct Camera {
    vec3 pos;
    vec3 dir;
    vec3 up;
    vec3 right;
};

layout(location = 16) uniform Camera camera;

vec2 rand_state;

float rand(){
    rand_state += vec2(1, -4);
    return fract(sin(dot(rand_state.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

bool inVolume(in vec3 pos) {
    return pos.x > domain_box_lower.x && pos.y > domain_box_lower.y &&
    pos.z > domain_box_lower.z && pos.x < domain_box_upper.x &&
    pos.y < domain_box_upper.y && pos.z < domain_box_upper.z;
}

float getExtinction(in vec3 pos) {
    if (!inVolume(pos)) {
        return max_extinction;
    }
    return texture(extinction_tex, pos).x;
}

bool sampleInteraction(inout vec3 ray_pos, in vec3 ray_dir) {
    float t = 0.0f;
    vec3 pos;
    for (int step = 0; step < 10; ++step) {
        t -= log(1.0f - rand()) / max_extinction;
        pos = ray_pos + ray_dir * t;
        if (!inVolume(pos)) {
            return false;
        }
        if (getExtinction(pos) >= rand() * max_extinction) {
            break;
        }
    }
    ray_pos = pos;
    return true;
}

bool intersectVolumeBox(out float tmin, in vec3 raypos, in vec3 raydir) {
    const float x0 = (0.f - raypos.x) / raydir.x;
    const float y0 = (0.f - raypos.y) / raydir.y;
    const float z0 = (0.f - raypos.z) / raydir.z;
    const float x1 = (1.f - raypos.x) / raydir.x;
    const float y1 = (1.f - raypos.y) / raydir.y;
    const float z1 = (1.f - raypos.z) / raydir.z;

    tmin = max(max(max(min(z0, z1), min(y0, y1)), min(x0, x1)), 0.0f);
    const float tmax = min(min(max(z0, z1), max(y0, y1)), max(x0, x1));
    return (tmin < tmax);
}

vec3 traceVolume(in vec3 ray_pos, in vec3 ray_dir) {
    float t0 = 0;
    float w = 1.0f;

    if (intersectVolumeBox(t0, ray_pos, ray_dir)) {
        ray_pos += ray_dir * t0;
        int interaction_i = 0;
        for (; interaction_i < max_interactions; ++interaction_i) {
            if (!sampleInteraction(ray_pos, ray_dir)) {
                break;
            }
            w *= albedo;
            // Russian roulette absorption
            if (w < 0.2f) {
                if (rand() > w * 5.0f) {
                    return vec3(1.0f, 0.0f, 0.0f);
                }
                w = 0.2f;
            }

            // Sample isotropic phase function
            const float phi = (2.0 * M_PI) * rand();
            const float cos_theta = 1.0f - 2.0f * rand();
            const float sin_theta = sqrt (1.0f - cos_theta * cos_theta);
            ray_dir = vec3(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);
        }
        // Is the path length exceeded?
        if (interaction_i >= max_interactions) {
            return vec3(0.0, 0.0, 0.0);
        }
    }
    const float f = (0.5f + 0.5f * ray_dir.y) * w;
    return vec3(f, f, f);
}

void main() {
    // assuming it is a unit cube
    vec3 ray_pos = camera.pos;
    vec3 ray_dir = normalize(tex-camera.pos);

    rand_state = ray_dir.xy + ray_dir.yz;

    vec3 value = traceVolume(ray_pos, ray_dir);

    for (int i = 1; i <= max_iterations; ++i) {
        vec3 u = rand() * 0.01 * camera.up;
        vec3 r = rand() * 0.01 * camera.right;
        vec3 cur_ray_dir = normalize(ray_dir + u + r);
        vec3 new_value  = traceVolume(ray_pos, cur_ray_dir);
        value += (new_value - value) / (i + 1);
    }

    //    value *= 0.2;

    //    value.x *= (1.0f + value.x * 0.1f) / (1.0f + value.x);
    //    value.y *= (1.0f + value.y * 0.1f) / (1.0f + value.y);
    //    value.z *= (1.0f + value.z * 0.1f) / (1.0f + value.z);

    //    value.x = pow(value.x, 1. / 2.2);
    //    value.y = pow(value.y, 1. / 2.2);
    //    value.z = pow(value.z, 1. / 2.2);

    fragColor = vec4(1,1,1,value.x);
}