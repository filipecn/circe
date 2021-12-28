#version 440 core


layout(std430, binding = 0) buffer layoutName
{
    struct {
        int face;
        int edge_vertex;
    } SSBO_data[];
};

out vec4 frag_color;

in vec3 bc;

uniform float edge_width;
uniform float vertex_width;

float factor(in float w) {
    vec3 d = fwidth(bc);
    vec3 f = step(d * w, bc);
    return min(min(f.x, f.y), f.z);
}

void main() {
    frag_color = vec4(bc, 1.0);

    if (SSBO_data[gl_PrimitiveID].face > 0 && SSBO_data[gl_PrimitiveID].edge_vertex == 0) {
        frag_color = vec4(1, 0, 1, 1);
    }

    if (factor(edge_width) < 1) {
        frag_color = vec4(0, 0, 0, 1);
    }

    int picked_edge = SSBO_data[gl_PrimitiveID].edge_vertex / 10;
    int picked_vertex = SSBO_data[gl_PrimitiveID].edge_vertex % 10;
    vec3 d = fwidth(bc);
    if (picked_vertex > 0) {
        vec3 f = step(d * vertex_width, bc);
        if (f.x > 0.9 || f.y > 0.9 || f.z > 0.9) {
            int vertex_id = 0;
            if (bc.x > 0.9) {
                vertex_id = 1;
            } else if (bc.y > 0.9) {
                vertex_id = 2;
            } else if (bc.z > 0.9) {
                vertex_id = 3;
            }
            if (vertex_id == picked_vertex) {
                frag_color = vec4(1, 1, 0, 1);
                return;
            }
        }
    }
    if (picked_edge > 0) {
        // check edge intersection
        vec3 f = step(d * edge_width, bc);
        if (min(min(f.x, f.y), f.z) < 1) {
            int edge_id = 3;
            if (f.x < f.y && f.x < f.z) {
                edge_id = 1;
            }
            else if (f.y < f.z && f.y < f.z) {
                edge_id = 2;
            }
            if (edge_id == picked_edge) {
                frag_color = vec4(1, 1, 1, 1);
                return;
            }
        }
    }
}
