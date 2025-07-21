#version 330 core

//#define VERTEX_WARPING

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec2 i_tex_coord;
layout (location = 2) in vec3 i_normal;

out VS_OUT
{
    vec2 tex_coord;
    vec3 normal;
    vec3 frag_pos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

#ifdef VERTEX_WARPING

vec4 warp_vertex(vec4 a)
{
    const float n = 0.01;
    return vec4(int(a.x / n) * n, int(a.y / n) * n, int(a.z / n) * n, a.w);
}

#endif      //VERTEX_WARPING

void main()
{
    vec4 pos = projection * view * model * vec4(i_pos, 1.0);

#ifdef VERTEX_WARPING

    pos = warp_vertex(pos);

#endif      //VERTEX_WARPING

    gl_Position = pos;

    vs_out.normal = mat3(transpose(inverse(model))) * i_normal;
    vs_out.tex_coord = i_tex_coord;
    vs_out.frag_pos = vec3(model * vec4(i_pos, 1.0));
}
