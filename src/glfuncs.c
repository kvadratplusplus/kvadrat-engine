#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>

#include <glad/glad.h>
#include <stb_image.h>

#ifdef __WIN32
#include <windows.h>
#endif
#ifdef __linux__
#include <linux/limits.h>
#endif

#include <defines.h>
#include <log_man.h>
#include <file_man.h>
#include <memory_man.h>

extern Config main_config;
extern Light * lights;
extern Camera main_camera;
extern char * cwd;
extern DirLight dir_light;

//precompiled missing shaders
static GLuint missing_vs;
static GLuint missing_fs;
static GLuint missing_sp;

/*
void glf_init(void)
{
    glf_precompile_missing();
}
    */

//this function should only be called when the engine is started
void glf_precompile_missing(void)
{
    GLchar *buf = NULL;
    FILE *vs_file = NULL;
    FILE *fs_file = NULL;
    GLuint vs_shader = 0;
    GLuint fs_shader = 0;
    GLuint shader_program = 0;

    allocate_mem((void**)&buf, sizeof(GLchar), 1024);

    if (!open_file(&vs_file, "shaders/missing.vert", "r")) {
        log_log(LOG_ERROR, "Shader file \"shaders/missing.vert\" does not exist", NULL);
        exit(EXIT_FAILURE);
    }
    fread(buf, sizeof(GLchar), 1024, vs_file);
    fclose(vs_file);
    buf[1023] = '\0';

    vs_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs_shader, 1, (const GLchar *const *)&buf, NULL);
    glCompileShader(vs_shader);

    int success;
    char log[512];
    glGetShaderiv(vs_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vs_shader, 512, NULL, log);
        log_log(
            LOG_ERROR,
            "GLSL Error: Vertex shader \"shaders/missing.vert\" compilation error:\n%s",
            log
        );
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < 1024; i++)
        buf[i] = '\0';

    if (!open_file(&fs_file, "shaders/missing.frag", "r")) {
        log_log(LOG_ERROR, "Shader file \"shaders/missing.frag\" does not exist", NULL);
        exit(EXIT_FAILURE);
    }
    fread(buf, sizeof(GLchar), 1024, fs_file);
    fclose(fs_file);
    buf[1023] = '\0';

    fs_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs_shader, 1, (const GLchar *const *)&buf, NULL);
    glCompileShader(fs_shader);

    glGetShaderiv(fs_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fs_shader, 512, NULL, log);
        log_log(
            LOG_ERROR,
            "GLSL Error: Fragment shader \"shaders/missing.frag\" compilation error:\n%s",
            log
        );
        exit(EXIT_FAILURE);
    }
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vs_shader);
    glAttachShader(shader_program, fs_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, log);
        log_log(LOG_ERROR, "GLSL Error: Shader program (VS: \"shaders/missing.vert\", FS: \"shaders/missing.frag\") compilation error:\n%s", log);
        exit(EXIT_FAILURE);
    }
    missing_vs = vs_shader;
    missing_fs = fs_shader;
    missing_sp = shader_program;
}

void glf_clear_missing(void)
{
    glDeleteShader(missing_vs);
    glDeleteShader(missing_fs);
    glDeleteProgram(missing_sp);
}

static inline int check_index(size_t * indeces, const size_t current, const size_t curr_shader_light)
{
    for (size_t i = 0; i < curr_shader_light; i++)
        if (indeces[i] == current)
            return 1;

    return 0;
}

//TODO move to scene.c
void glf_find_nearest_lights(kvec3 * position, size_t * buffer)
{
    float most_near_dist = FLT_MAX;     //last nearest

    for (size_t i = 0; i < main_config.shader_lights_count; i++) {
        most_near_dist = FLT_MAX;
        for (size_t j = 0; j < main_config.lights_max; j++) {
            if (check_index(buffer, j, i) || !lights[j].is_active)
                continue;

            float dist = kvec_distance(position, &lights[j].position);
            if (most_near_dist > dist) {
                most_near_dist = dist;
                buffer[i] = j;
            }
        }
    }
}

void glf_load_light_pos(size_t * buffer, GLuint shader_program)
{
    glUseProgram(shader_program);

    glUniform3f(glGetUniformLocation(shader_program, "dir_light.direction"),
        dir_light.direction.x, dir_light.direction.y, dir_light.direction.z);

    glUniform3f(glGetUniformLocation(shader_program, "dir_light.color"),
        dir_light.color.x, dir_light.color.y, dir_light.color.z);

    glUniform3f(glGetUniformLocation(shader_program, "dir_light.ambient_color"),
        dir_light.ambient_color.x, dir_light.ambient_color.y, dir_light.ambient_color.z);

    char str[64];
    for (size_t i = 0; i < main_config.shader_lights_count; i++) {
        sprintf(str, "lights[%zu].position", i);
        glUniform3f(glGetUniformLocation(shader_program, str),
            lights[buffer[i]].position.x, lights[buffer[i]].position.y, lights[buffer[i]].position.z);

        sprintf(str, "lights[%zu].color", i);
        glUniform3f(glGetUniformLocation(shader_program, str),
            lights[buffer[i]].color.x, lights[buffer[i]].color.y, lights[buffer[i]].color.z);

        sprintf(str, "lights[%zu].constant", i);
        glUniform1f(glGetUniformLocation(shader_program, str), lights[buffer[i]].constant);

        sprintf(str, "lights[%zu].linear", i);
        glUniform1f(glGetUniformLocation(shader_program, str), lights[buffer[i]].linear);

        sprintf(str, "lights[%zu].quadratic", i);
        glUniform1f(glGetUniformLocation(shader_program, str), lights[buffer[i]].quadratic);
    }
}

void glf_draw(GLuint shader_program, GLuint texture,
    GLuint vao, kmat4 * model_matrix, size_t start_index, size_t end_index)
{
    glUseProgram(shader_program);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"),
        1, GL_FALSE, &model_matrix->x1);
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"),
        1, GL_FALSE, &main_camera.view_matrix.x1);
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"),
        1, GL_FALSE, &main_camera.projection_matrix.x1);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, start_index, end_index);

    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void glf_load_buffers(GLuint * vao, GLuint * vbo,
    size_t vertex_count, float * vertex_array)
{
    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VERTICES_ATTRIBS * vertex_count,
    vertex_array, GL_STATIC_DRAW);

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTICES_ATTRIBS * sizeof(float),
    (void *)0);
    glEnableVertexAttribArray(0);
    //texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTICES_ATTRIBS * sizeof(float),
    (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //normal vector attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTICES_ATTRIBS * sizeof(float),
    (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

//returns shader object
//type = GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
GLuint glf_load_shader(char *file_name, const int type)
{
    static uint8_t first_time = 1;
    static char * shader_content;
    if (first_time) {
        first_time = 0;
        allocate_mem((void**)&shader_content, sizeof(char), main_config.shader_chars_max);
    }
    FILE * shader_file = NULL;
    if (!open_file(&shader_file, file_name, "r")) {
        log_log(LOG_WARNING, "Shader file \"%s\" does not exist, loading missing shader", file_name);
        // char missing_name[32];

        // if (type == GL_VERTEX_SHADER)
        //     strcpy(missing_name, "shaders/missing.vert");

        // if (type == GL_FRAGMENT_SHADER)
        //     strcpy(missing_name, "shaders/missing.frag");

        // if (!open_file(&shader_file, missing_name, "r")) {
        //     log_log(LOG_ERROR, "Missing shader is missing", NULL);
        //     exit(EXIT_FAILURE);
        // }
        return type == GL_VERTEX_SHADER ? missing_vs : missing_fs;
    }
    fseek(shader_file, 0, SEEK_END);
    long length = ftell(shader_file);
    rewind(shader_file);

    if (length > main_config.shader_chars_max - 1) {
        log_log(LOG_ERROR, "Shader file \"%s\" is too long (%lld / %zu)",
            file_name, length, main_config.shader_chars_max);
        fclose(shader_file);
        exit(EXIT_FAILURE);
    } else {
        if (fread(shader_content, sizeof(char), length, shader_file) != length) {
            log_log(LOG_ERROR, "Failed to read file \"%s\"", file_name);
            fclose(shader_file);
            exit(EXIT_FAILURE);
        }
        shader_content[length] = '\0';
    }
    fclose(shader_file);

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar *const *)&shader_content, NULL);
    glCompileShader(shader);

    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, log);
        log_log(
            LOG_ERROR,
            "GLSL Error: %s shader \"%s\" compilation error:\n%s",
            type == GL_VERTEX_SHADER ? "Vertex" : "Fragment",
            file_name,
            log
        );
        exit(EXIT_FAILURE);
    }
    return shader;
}

//returns shader program
GLuint glf_load_shader_program(char * vertex_shader_name, char * fragment_shader_name)
{
    GLuint vertex_shader =
        strcmp(vertex_shader_name, "shaders/missing.vert") == 0 ?
        missing_vs :
        glf_load_shader(vertex_shader_name, GL_VERTEX_SHADER);
    GLuint fragment_shader =
        strcmp(fragment_shader_name, "shaders/missing.frag") == 0 ?
        missing_fs :
        glf_load_shader(fragment_shader_name, GL_FRAGMENT_SHADER);

    if (vertex_shader == missing_vs && fragment_shader == missing_fs)
        return missing_sp;

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    int success;
    char log[512];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, log);
        log_log(LOG_ERROR, "GLSL Error: Shader program (VS: \"%s\", FS: \"%s\") compilation error:\n%s",
            vertex_shader_name, fragment_shader_name, log);
        exit(EXIT_FAILURE);
    }
    //delete shaders
    if (vertex_shader != missing_vs)
        glDeleteShader(vertex_shader);

    if (fragment_shader != missing_fs)
        glDeleteShader(fragment_shader);

    return shader_program;
}

//TODO .ktf support, split to load_tex (fileman.c) and glf_load_tex
//returns texture object
GLuint glf_load_texture(char * texture_name)
{
    static uint8_t * data = NULL;
    static uint8_t first_time = 1;

    int width = 64;
    int height = 64;
    //int nrChannels = 3;
    GLuint texture_object = 0;
    char full_name[PATH_MAX] = {0};
    FILE * file = NULL;

    if (first_time) {
        allocate_mem((void**)&data, 1, 64 * 64 * 3 + 10);
        first_time = 0;
    }

    snprintf(full_name, PATH_MAX, "%s/%s", cwd, texture_name);
//    uint8_t * data = stbi_load(full_name, &width, &height, &nrChannels, 0);
    if (!open_file(&file, full_name, "rb"))
            ;       //TODO error handler

    fread(data, 1, 64 * 64 * 3, file);
    fclose(file);

    glGenTextures(1, &texture_object);
    glBindTexture(GL_TEXTURE_2D, texture_object);

    //point filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /*
    if (!data) {
        log_log(LOG_WARNING, "Failed to load texture \"%s\", loading missing texture", texture_name);
        char path[PATH_MAX + 1] = {0};
        snprintf(path, PATH_MAX, "%s/%s", cwd, "textures/missing.png");

 //       data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (!data) {
            log_log(LOG_ERROR, "Missing texture is missing", NULL);
            exit(EXIT_FAILURE);
        }
    }
    */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture_object;
}
