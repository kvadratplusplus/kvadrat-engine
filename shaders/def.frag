#version 330 core

#define LIGHTS_COUNT 4

#define DEFAULT
//#define NO_LIGHT
//#define TEST
//#define DISPLAY_NORMALS
//#define DISPLAY_TEX_COORDS

out vec4 frag_color;
  
in VS_OUT
{
    vec2 tex_coord;
    vec3 normal;
    vec3 frag_pos;
} fs_in;

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

uniform sampler2D sample_texture;
uniform Light lights[LIGHTS_COUNT];
uniform DirLight dir_light;

#ifdef DEFAULT

vec3 calc_global_diff()
{
    float diff = max(dot(normalize(fs_in.normal), normalize(-dir_light.direction)), 0.0);
    return diff * dir_light.color;
}

vec3 calc_light_diff(Light light)
{
    if (light.constant == 0.01)
        return (light.color);

    float diff = max(dot(normalize(fs_in.normal), normalize(light.position - fs_in.frag_pos)), 0.0);
    float dist = distance(light.position, fs_in.frag_pos);
    float att = light.constant + light.linear / dist + light.quadratic / (dist * dist);

    return (light.color * diff * att);
}

#endif      //DEFAULT

void main()
{

#ifdef DEFAULT

    vec3 lightning = dir_light.ambient_color + calc_global_diff();
    
    for (int i = 0; i < LIGHTS_COUNT; i++)
        lightning += calc_light_diff(lights[i]);

    lightning *= texture(sample_texture, fs_in.tex_coord).rgb;
    frag_color = vec4(lightning, 1.0);

#endif      //DEFAULT

#ifdef NO_LIGHT

    frag_color = vec4(texture(sample_texture, fs_in.tex_coord).rgb, 1.0);

#endif      //NO_LIGHT

#ifdef TEST

    vec3 color = vec3(dot(fs_in.normal, normalize(vec3(0.5, 1, 0.2))));
    if (color.x < 0.1)
    {
        color = vec3(0.1);
    }

    frag_color = vec4(color, 1.0);

#endif      //TEST

#ifdef DISPLAY_NORMALS

    frag_color = vec4(abs(fs_in.normal), 1.0);

#endif      //DISPLAY_NORMALS

#ifdef DISPLAY_TEX_COORDS

    frag_color = vec4(abs(fs_in.tex_coord), 0.0, 1.0);

#endif      //DISPLAY_TEX_COORDS

}
