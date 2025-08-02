#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include <file_man.h>
#include <defines.h>
#include <log_man.h>

extern Config main_config;

void config_read_args(const char ** argv, int argc)
{
    for (size_t i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-log") == 0) {
            main_config.log = 1;
            log_init();
            log_log(LOG_LOG, "Log started", NULL);
        } else if (strcmp(argv[i], "-fps") == 0) {
            main_config.fps = 1;
        } else if (strcmp(argv[i], "-unlim") == 0) {
            main_config.unlim = 1;
        }

        else {
            log_log(LOG_WARNING, "Unknown command line argument: \"%s\"", argv[i]);
        }
    }
}

void config_read(char * name)
{
    FILE * file = NULL;
    char word[32] = {0};

    if(!open_file(&file, name, "r")) {
        log_log(LOG_ERROR, "Config file does not exist", NULL);
        exit(EXIT_FAILURE);
    }
    while (fscanf(file, "%s", word) != EOF) {
        if (strcmp(word, "models_max") == 0)
            fscanf(file, "%u", &main_config.models_max);

        else if (strcmp(word, "lights_max") == 0)
            fscanf(file, "%u", &main_config.lights_max);

        else if (strcmp(word, "vertices_max") == 0)
            fscanf(file, "%u", &main_config.vertices_max);

        else if (strcmp(word, "fullscreen") == 0)
            fscanf(file, "%hhu", &main_config.fullscreen);

        else if (strcmp(word, "screen_height") == 0)
            fscanf(file, "%d", &main_config.screen_height);

        else if (strcmp(word, "screen_width") == 0)
            fscanf(file, "%d", &main_config.screen_width);

        else if (strcmp(word, "shader_chars_max") == 0)
            fscanf(file, "%u", &main_config.shader_chars_max);

        else if (strcmp(word, "shader_lights_count") == 0)
            fscanf(file, "%u", &main_config.shader_lights_count);

        else if (strcmp(word, "sectors_max") == 0)
            fscanf(file, "%u", &main_config.sectors_max);

        else if (strcmp(word, "texsize_max") == 0)
            fscanf(file, "%u", &main_config.texsize_max);

        else if (strcmp(word, "buf_prog") == 0)
            fscanf(file, "%u", &main_config.buf_prog);

        else if (strcmp(word, "buf_tex") == 0)
            fscanf(file, "%u", &main_config.buf_tex);

        else if (strcmp(word, "buf_vbo") == 0)
            fscanf(file, "%u", &main_config.buf_vbo);

        else
            log_log(LOG_WARNING, "Unknown config parameter \"%s\"", word);

    }
    fclose(file);
    if (main_config.models_max == 0) {
        log_log(LOG_WARNING, "models_max = 0, setting to 128", NULL);
        main_config.models_max = 128;
    }
    if (main_config.lights_max == 0) {
        log_log(LOG_WARNING, "lights_max = 0, setting to 128", NULL);
        main_config.lights_max = 128;
    }
    if (main_config.vertices_max == 0) {
        log_log(LOG_WARNING, "vertices_max = 0, setting to 128", NULL);
        main_config.vertices_max = 128;
    }
    if (main_config.screen_height < 128) {
        log_log(LOG_WARNING, "screen_height < 128, setting to 128", NULL);
        main_config.screen_height = 128;
    }
    if (main_config.screen_width < 128) {
        log_log(LOG_WARNING, "screen_width < 128, setting to 128", NULL);
        main_config.screen_width = 128;
    }
    if (main_config.shader_chars_max == 0) {
        log_log(LOG_WARNING, "shader_chars_max = 0, setting to 4096", NULL);
        main_config.shader_chars_max = 4096;
    }
    if (main_config.shader_lights_count == 0) {
        log_log(LOG_WARNING, "shader_lights_count = 0, setting to 4", NULL);
        main_config.shader_lights_count = 4;
    }
    if (main_config.sectors_max == 0) {
        log_log(LOG_WARNING, "sectors_max = 0, setting to 128", NULL);
        main_config.sectors_max = 128;
    }
    if (main_config.texsize_max == 0) {
        log_log(LOG_WARNING, "texsize_max = 0, setting to 1024", NULL);
        main_config.texsize_max = 1024;
    }
    if (main_config.buf_prog == 0) {
        log_log(LOG_WARNING, "buf_prog = 0, setting to 32", NULL);
        main_config.buf_prog = 32;
    }
    if (main_config.buf_tex == 0) {
        log_log(LOG_WARNING, "buf_tex = 0, setting to 512", NULL);
        main_config.buf_tex = 512;
    }
    if (main_config.buf_vbo == 0) {
        log_log(LOG_WARNING, "buf_vbo = 0, setting to 512", NULL);
        main_config.buf_vbo = 512;
    }
}
