#pragma once

#include <stdint.h>

#include <kmatrix.h>
#include <defines.h>

#define ENGINE_VERSION 1

//pos(x,y,z),tex_coords(x,y),normal(x,y,z)
#define VERTICES_ATTRIBS 8

//text
#define TEXT_ERROR "\033[1;31mError: \033[0m"
#define TEXT_WARNING "\033[1;33mWarning: \033[0m"
#define TEXT_DEFAULT "\033[0m"
#define TEXT_RED "\033[31m"
#define TEXT_BOLD "\033[1m"
#define TEXT_YELLOW "\033[33m"

typedef struct
{
    uint32_t vertices_max;
    uint32_t models_max;
    uint32_t lights_max;
    uint32_t sectors_max;
    uint32_t shader_chars_max;
    uint32_t shader_lights_count;
    uint32_t screen_width;
    uint32_t screen_height;
    
    uint8_t fullscreen;
    uint8_t log;
    uint8_t unlim;
    uint8_t fps;
} Config;

typedef struct
{
    float time;
    float delta_time;

    float sin_time;
    float cos_time;
} kTime;

typedef struct
{
    kmat4 projection_matrix;
    kmat4 view_matrix;

    kvec3 camera_position;
    kvec3 camera_front;
    kvec3 camera_up;

    float camera_pitch;
    float camera_yaw;
} Camera;

typedef struct
{
    kvec3 ambient_color;
    kvec3 direction;
    kvec3 color;
} DirLight;

typedef struct
{
    kvec3 position;
    kvec3 color;

    float constant;
    float linear;
    float quadratic;

    uint8_t is_active;
} Light;

typedef struct
{
    uint8_t portal[4];

    kvec2 points[4];
    kvec3 center;
    float top;
    float bottom;

    uint32_t vao;
    uint32_t vbo;
    uint32_t textures[6];
    uint32_t shader_programs[6];
    
    uint8_t is_active;
} Sector;

typedef struct
{
    kmat4 model_matrix;

    uint32_t vao;
    uint32_t vbo;
    uint32_t shader_program;
    uint32_t vertex_count;
    uint32_t texture;

    uint8_t is_active;
} Model;
