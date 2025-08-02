#pragma once

#include <stdint.h>

#include <kmatrix.h>

uint32_t glf_load_shader_program(uint32_t vert, uint32_t frag);
uint32_t glf_load_shader(char * content, const int type);
uint32_t glf_load_tex(uint8_t * data, uint32_t width, uint32_t height);
void glf_load_buffers(uint32_t * vao, uint32_t * vbo,
    size_t vertex_count, float * vertex_array);

void glf_draw(uint32_t shader_program, uint32_t texture,
    uint32_t vao, kmat4 * model_matrix, size_t start_index,
    size_t end_index);
void glf_load_light_pos(size_t * buffer, uint32_t shader_program);
void glf_find_nearest_lights(kvec3 * position, size_t * buffer);

//uint32_t glf_load_texture(char * texture_name);
//void glf_precompile_missing(void);
//void glf_clear_missing(void);
