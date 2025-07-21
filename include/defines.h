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
    size_t vertices_max;
    size_t models_max;
    size_t lights_max;
    size_t sectors_max;
    size_t shader_chars_max;
    size_t shader_lights_count;
    
    int screen_width;
    int screen_height;
    
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

    unsigned int vao;
    unsigned int vbo;
    unsigned int textures[6];
    unsigned int shader_programs[6];
    
    uint8_t is_active;
} Sector;

typedef struct
{
    unsigned int vao;
    unsigned int vbo;
    unsigned int shader_program;
    unsigned int vertex_count;
    unsigned int texture;
    kmat4 model_matrix;
    uint8_t is_active;
} Model;

/*
typedef struct
{
    kvec2 start;
    kvec2 end;
    float top;
    float bottom;

    Model model;

    //uint8_t is_active;
} Wall;
*/
