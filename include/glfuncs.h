#pragma once

#include <stdint.h>

#include <kmatrix.h>

void glf_precompile_missing(void);
void glf_clear_missing(void);
void glf_load_buffers(unsigned int * vao, unsigned int * vbo, size_t vertex_count, float * vertex_array);
void glf_load_buffers(unsigned int * vao, unsigned int * vbo, size_t vertex_count, float * vertex_array);
void glf_draw(unsigned int shader_program, unsigned int texture, unsigned int vao, kmat4 * model_matrix, size_t start_index, size_t end_index);
void glf_load_light_pos(size_t * buffer, unsigned int shader_program);
void glf_find_nearest_lights(kvec3 * position, size_t * buffer);
unsigned int glf_load_texture(char * texture_name);
unsigned int glf_load_shader_program(char * vertex_shader_name, char * fragment_shader_name);
unsigned int glf_load_shader(char *file_name, const int type);
