#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <defines.h>
#include <memory_man.h>
#include <file_man.h>
#include <log_man.h>
#include <sector.h>
#include <glfuncs.h>

extern DirLight dir_light;
extern Config main_config;

extern Light * lights;
extern Sector * sectors;
extern Model * models;

void scene_load_scene(char * scene_name)
{
    static uint8_t first_time = 1;
    static float * vertex_buffer;
    if (first_time) {
        first_time = 0;
        allocate_mem((void**)&vertex_buffer, sizeof(float), main_config.vertices_max * VERTICES_ATTRIBS);
    }
    FILE * scene_file = NULL;
    if(!open_file(&scene_file, scene_name, "r")) {
        log_log(LOG_ERROR, "Scene file \"%s\" does not exist", scene_name);
        exit(EXIT_FAILURE);
    }
    char word[64] = {0};
    size_t current_light = 0;
    size_t current_sector = 0;
    size_t current_model = 0;

    while (fscanf(scene_file, "%63s", word) != EOF) {
        if (strcmp(word, "dir_light") == 0) {
            fscanf(
                scene_file, "%f %f %f %f %f %f %f %f %f",
                &dir_light.direction.x,
                &dir_light.direction.y,
                &dir_light.direction.z,
                &dir_light.ambient_color.x,
                &dir_light.ambient_color.y,
                &dir_light.ambient_color.z,
                &dir_light.color.x,
                &dir_light.color.y,
                &dir_light.color.z
            );
        } else if (strcmp(word, "light") == 0) {
            if (current_light > main_config.lights_max - 1) {
                log_log(LOG_WARNING, "Scene \"%s\" has too many lights\n", scene_name);
                continue;
            }
            fscanf(
                scene_file,
                "%f %f %f %f %f %f %f %f %f",
                &lights[current_light].position.x,
                &lights[current_light].position.y,
                &lights[current_light].position.z,
                &lights[current_light].color.x,
                &lights[current_light].color.y,
                &lights[current_light].color.z,
                &lights[current_light].constant,
                &lights[current_light].linear,
                &lights[current_light].quadratic
            );
            lights[current_light].is_active = 1;
            
            current_light++;
        } else if (strcmp(word, "sector") == 0) {
            SectorLayout layout;

            if (current_sector >= main_config.sectors_max) {
                log_log(LOG_WARNING, "Scene \"%s\" has too many sectors\n",
                    scene_name);
                continue;
            }
            fscanf(scene_file, "%f %f", &layout.bottom, &layout.top);

            for (size_t i = 0; i < 4; i++)
                fscanf(scene_file, "%f", &layout.connection_top[i]);

            for (size_t i = 0; i < 4; i++)
                fscanf(scene_file, "%f", &layout.connection_bottom[i]);

            for (size_t i = 0; i < 4; i++)
                fscanf(scene_file, "%f %f", &layout.points[i].x, &layout.points[i].y);

            for (size_t i = 0; i < 4; i++)
                fscanf(scene_file, "%hhu", &layout.portal[i]);

            for (size_t i = 0; i < 6; i++)
                fscanf(scene_file, "%127s", layout.texture_names[i]);

            for (size_t i = 0; i < 6; i++)
                fscanf(scene_file, "%127s", layout.vertex_shader_names[i]);

            for (size_t i = 0; i < 6; i++)
                fscanf(scene_file, "%127s", layout.fragment_shader_names[i]);

            for (size_t i = 0; i < 6; i++)
                fscanf(scene_file, "%f %f", &layout.tex_coords[i].x, &layout.tex_coords[i].y);

            sector_create(&layout, &sectors[current_sector]);
            sectors[current_sector].is_active = 1;

            current_sector++;
        } else if (strcmp(word, "model") == 0) {
            if (current_model > main_config.models_max - 1) {
                log_log(LOG_WARNING, "Scene \"%s\" has too many models\n", scene_name);
                continue;
            }
            char vertex_shader_name[64] = {0};
            char fragment_shader_name[64] = {0};
            char model_name[64] = {0};
            char texture_name[64] = {0};
            float x, y, z;

            fscanf(
                scene_file,
                "%63s %63s %63s %63s %f %f %f",
                model_name,
                vertex_shader_name,
                fragment_shader_name,
                texture_name,
                &x, &y, &z
            );
            load_model(model_name, vertex_buffer, &models[current_model]);
            glf_load_buffers(&models[current_model].vao, &models[current_model].vbo, models[current_model].vertex_count, vertex_buffer);
            models[current_model].shader_program = glf_load_shader_program(vertex_shader_name, fragment_shader_name);
            models[current_model].texture = glf_load_texture(texture_name);
            models[current_model].model_matrix = kmat_translate(&(kvec3){.x = x, .y = y, .z = z});
            models[current_model].is_active = 1;

            current_model++;
        } else {
            log_log(LOG_WARNING, "Unknown object type \"%s\" in scene file \"%s\"", word, scene_name);
        }
    }

    if (current_light < main_config.shader_lights_count)
        for (size_t i = 0; i < main_config.shader_lights_count; i++)
            lights[i].is_active = 1;
}
