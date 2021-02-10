#version 440 core

layout(std430, binding = 0) buffer layoutName
{
    struct {
        vec3 color;
        float alpha;
    } SSBO_data[];
};

void main() {
    gl_FragColor = vec4(SSBO_data[gl_PrimitiveID].color, SSBO_data[gl_PrimitiveID].alpha);
}
