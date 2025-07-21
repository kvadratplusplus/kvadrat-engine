#version 330 core
layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec2 i_tex_coord;
layout (location = 2) in vec3 i_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec4 warp_vertex(vec4 a)
{
    const float n = 0.1;
    return vec4(int(a.x / n) * n, int(a.y / n) * n, int(a.z / n) * n, a.w);
}

void main()
{
    vec4 pos = projection * view * model * vec4(i_pos, 1.0);
    pos = warp_vertex(pos);
    gl_Position = pos;
}
