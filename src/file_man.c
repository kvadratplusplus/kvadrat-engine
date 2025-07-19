#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//for PATH_MAX
#ifdef __WIN32
#include <windows.h>
#endif
#ifdef __linux__
#include <linux/limits.h>
#endif

#include "defines.h"
#include "file_man.h"
#include "log_man.h"

extern char * cwd;
extern Config main_config;

void load_model(char *name, float *array, Model *model)
{
    FILE * model_file = NULL;
    if (!open_file(&model_file, name, "rb")) {
        log_log(LOG_WARNING, "Model file \"%s\" does not exist, loading missing model", name);
        if(!open_file(&model_file, "models/missing.kmdl", "rb")) {
            log_log(LOG_ERROR, "Missing model is missing", NULL);
            exit(EXIT_FAILURE);
        }
    }
    fseek(model_file, 0, SEEK_END);
    long length = ftell(model_file);
    rewind(model_file);

    size_t elements_count = length / sizeof(float);
    if (elements_count > (main_config.vertices_max - 1) * VERTICES_ATTRIBS) {
        log_log(
            LOG_WARNING,
            "Model \"%s\" has too many vertices (%zu / %zu)",
            name,
            elements_count / VERTICES_ATTRIBS,
            main_config.vertices_max
        );
        elements_count = (main_config.vertices_max - 1) * VERTICES_ATTRIBS;
    }
    fread(array, sizeof(float), elements_count, model_file);
    model->vertex_count = elements_count / VERTICES_ATTRIBS;
}

int open_file(FILE ** file, char * name, char * fopen_mode)
{
    char full_name[PATH_MAX];
    //real_path(name, full_name);
    //if ((strlen(name) + strlen(cwd) + 2) >= PATH_MAX)
    //    return;
    //else
    snprintf(full_name, PATH_MAX, "%s/%s", cwd, name);

    *file = fopen(full_name, fopen_mode);
    if (!*file) {
        log_log(LOG_ERROR, "Failed to open file \"%s\" (\"%s\")", name, full_name);
        return 0;       //fail
    }

    return 1;       //success
}

/*
static void real_path(char * name, char * buffer)
{
    if ((strlen(name) + strlen(cwd) + 2) >= PATH_MAX)
        return;
    else
        snprintf(buffer, PATH_MAX, "%s/%s", cwd, name);
}
*/

// /[][][][][]/[][][][][]/[]
// ^                     ^
// full_path             last_sep
// /[][][]
// ^
// full_path = last_sep
void extract_directory(const char* full_path, char* dir_buffer, size_t buf_size)
{
    char * last_separator = strrchr(full_path, '/');

    if (last_separator == NULL)
        last_separator = strrchr(full_path, '\\');

    if (last_separator == NULL || last_separator == full_path) {
        //this is only possible on unix like systems
        memcpy(dir_buffer, "/", 2);
    } else {
        //this is only possible on windows
        size_t len_to_copy = last_separator - full_path;

        if (len_to_copy > buf_size - 1) {
            log_log(LOG_ERROR, "Buffer is too small", NULL);
            exit(EXIT_FAILURE);
        }
        memcpy(dir_buffer, full_path, len_to_copy);
        dir_buffer[len_to_copy] = '\0';
    }
}
