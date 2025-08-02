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

//TODO move to scene.c
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

void glf_load_buffers(GLuint * vao, GLuint * vbo, size_t vertex_count, float * vertex_array)
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

//type = GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
GLuint glf_load_shader(char * content, const int type)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar *const *)&content, NULL);
    glCompileShader(shader);

    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, log);
        log_log(
            LOG_ERROR,
            "GLSL Error: %s shader compilation error:\n%s",
            type == GL_VERTEX_SHADER ? "Vertex" : "Fragment",
            log
        );
        return 0;       //fail
    }
    return shader;      //success
}

GLuint glf_load_shader_program(GLuint vert, GLuint frag)
{
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);

    int success;
    char log[512];
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(prog, 512, NULL, log);
        log_log(
            LOG_ERROR,
            "GLSL Error: Shader program (VS: \"%s\", FS: \"%s\") compilation error:\n%s",
            vert, frag, log
        );
        return 0;       //fail
    }
    return prog;        //success
}

GLuint glf_load_tex(uint8_t * data, uint32_t width, uint32_t height)
{
    GLuint texture = 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //point filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;      //success
}
