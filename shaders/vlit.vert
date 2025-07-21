#version 330 core

#define LIGHTS_COUNT 4

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec2 i_tex_coord;
layout (location = 2) in vec3 i_normal;

out vec3 col;
out vec2 tex_coord;

vec4 pos = vec4(0);

struct DirLight
{
    vec3 ambient_color;
    vec3 direction;
    vec3 color;
};

struct Light
{
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D tex;
uniform Light lights[LIGHTS_COUNT];
uniform DirLight dir_light;

vec3 calc_global_diff()
{
    float diff = max(dot(normalize(i_normal), normalize(-dir_light.direction)), 0.0);
    return diff * dir_light.color;
}

vec3 calc_light_diff(Light light)
{
    float diff = max(dot(normalize(i_normal), normalize(light.position - pos.xyz)), 0.0);
    float dist = distance(light.position, i_pos);
    float att = light.constant + light.linear / dist + light.quadratic / (dist * dist);

    return (light.color * diff * att);
}

void main()
{
    pos = model * vec4(i_pos, 1.0);
    gl_Position = projection * view * pos;

    vec3 lightning = dir_light.ambient_color + calc_global_diff();
    for (int i = 0; i < LIGHTS_COUNT; i++)
        lightning += calc_light_diff(lights[i]);

    tex_coord = i_tex_coord;
    col = lightning;
}

