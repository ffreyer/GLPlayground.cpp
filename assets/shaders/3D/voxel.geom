#version 330 core

// TODO: 
// Use viewdir to discard invisible faces (reducing max_vertices to 12)

layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

flat in ivec3[] g_idx;

flat out vec3 f_normal;
out vec2 f_uv;
out vec4 f_shadow_pos;

uniform isampler3D block_id;

uniform mat4 projectionview;
uniform mat4 model;
uniform mat3 normalmatrix;
uniform mat4 lightspace;

// texture mapping
// TODO: 
// std140 packs things into vec4 which generates a mess here
// Can we do better? maybe with packed & querrying offsets?
layout(std140) uniform uv_lut_block {
    ivec4 uv_lut[16 * 4 * 6];
};
uniform vec2 tex_uv_size;
uniform ivec2 tex_index_size;

const vec2 TEX_OFFSETS[4] = vec2[](vec2(0, 0), vec2(1, 0), vec2(0, 1), vec2(1, 1));

vec2 get_uv(int id, int i) {
    ivec4 sub = uv_lut[id / 4];
    int idx = sub[int(mod(id, 4))];
    vec2 idx2d = vec2(mod(idx, tex_index_size.x), idx / tex_index_size.x);
    return tex_uv_size * (idx2d + TEX_OFFSETS[i]);
}

// Mesh generation constants
const int MAX_INDEX = 16;

const ivec3 OFFSETS[6] = ivec3[](
    ivec3(+1, 0, 0), ivec3(-1, 0, 0),
    ivec3(0, +1, 0), ivec3(0, -1, 0), 
    ivec3(0, 0, +1), ivec3(0, 0, -1)
);

const vec3 FACES[24] = vec3[24](
    vec3(1, 0, 0), vec3(1, 0, 1), vec3(1, 1, 0), vec3(1, 1, 1),
    vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 0), vec3(0, 1, 1),
    vec3(0, 1, 0), vec3(0, 1, 1), vec3(1, 1, 0), vec3(1, 1, 1),
    vec3(0, 0, 0), vec3(0, 0, 1), vec3(1, 0, 0), vec3(1, 0, 1),
    vec3(0, 0, 1), vec3(0, 1, 1), vec3(1, 0, 1), vec3(1, 1, 1),
    vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 0, 0), vec3(1, 1, 0)
);

const vec3 NORMALS[6] = vec3[6](
    vec3(+1, 0, 0), vec3(-1, 0, 0),
    vec3(0, +1, 0), vec3(0, -1, 0),
    vec3(0, 0, +1), vec3(0, 0, -1)
);

// Mesh/vertex gen

bool verify(ivec3 idx) {
    return 
        (0 <= idx.x) && (idx.x < MAX_INDEX) &&
        (0 <= idx.y) && (idx.y < MAX_INDEX) &&
        (0 <= idx.z) && (idx.z < MAX_INDEX);
}

void generate_face(int id, int neighbour_idx) {
    vec3 normal = normalize(normalmatrix * NORMALS[neighbour_idx]);
    vec3 center = vec3(g_idx[0]);
    for (int i = 0; i < 4; i++) {
        vec4 world_pos = model * vec4(center + FACES[4 * neighbour_idx + i], 1);
        gl_Position = projectionview * world_pos;
        f_normal = normal;
        f_uv = get_uv(6 * id + neighbour_idx, i);
        f_shadow_pos = lightspace * world_pos;
        EmitVertex();
    }
    EndPrimitive();
}

void maybe_render_face(int id, int neighbour_idx) {
    ivec3 idx = g_idx[0] + OFFSETS[neighbour_idx];

    // Always render Chunk edge && skip renderign faces occluded by other blocks
    int neighbor_id = 0;
    if (verify(idx)) {
        neighbor_id = texelFetch(block_id, idx, 0).r;
        if (neighbor_id != 0) return;
    }

    generate_face(id, neighbour_idx);
}

void main(){
    // Return immediately if this block is invisible (air)
    int id = texelFetch(block_id, g_idx[0], 0).r;
    if (id == 0)
        return;

    for (int i = 0; i < 6; i++)
        maybe_render_face(id, i);
}