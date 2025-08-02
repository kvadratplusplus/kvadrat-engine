#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <glad/glad.h>

#include <glfuncs.h>
#include <file_man.h>
#include <memory_man.h>
#include <log_man.h>
#include <defines.h>

struct tex
{
    char name[128];
    GLuint var;
};

struct mdl
{
    char name[128];
    GLuint vbo;
    GLuint vao;
    uint32_t v_count;
};

struct prg
{
    char vert[128];
    char frag[128];
    GLuint var;
};

extern Config main_config;
extern Light * lights;
extern Sector * sectors;
extern Model * models;

static GLuint mis_tex = 0;
static GLuint mis_vbo = 0;
static GLuint mis_vao = 0;
static GLuint mis_sp = 0;
static GLuint mis_vs = 0;
static GLuint mis_fs = 0;
static GLuint mis_mdl_vc = 0;

static struct prg * prgs = NULL;
static struct mdl * mdls = NULL;
static struct tex * texs = NULL;

static uint8_t * tex_cont = NULL;
static char * shd_cont = NULL;
static float * mdl_cont = NULL;

GLuint bufs_model(char * name, uint32_t * v_count, GLuint * vao)
{
    static size_t top = 0;
    FILE * file = NULL;
    GLuint model = 0;
    long l = 0;
    size_t elems = 0;

    for (size_t i = 0; i < main_config.buf_vbo; i++) {
        if (strcmp(name, mdls[i].name) == 0) {
            *v_count = mdls[i].v_count;
            *vao = mdls[i].vao;
            return mdls[i].vbo;
        }
    }
    if (!open_file(&file, name, "rb")) {
        log_log(LOG_WARNING, "File \"%s\" does not exist, loading \"models/missing.kmdl\"", name);
        *v_count = mis_mdl_vc;
        *vao = mis_vao;
        return mis_vbo;
    }
    fseek(file, 0, SEEK_END);
    l = ftell(file);
    rewind(file);
    elems = l / sizeof(float);
    if (elems > (main_config.vertices_max - 1) * VERTICES_ATTRIBS) {
        log_log(
            LOG_WARNING,
            "Model \"%s\" has too many vertices(%zu / %zu)",
            elems / VERTICES_ATTRIBS,
            main_config.vertices_max
        );
        *v_count = mis_mdl_vc;
        *vao = mis_vao;
        return mis_vbo;
    }
    fread(mdl_cont, sizeof(float), elems, file);
    fclose(file);
    *v_count = elems / VERTICES_ATTRIBS;
    glf_load_buffers(vao, &model, *v_count, mdl_cont);
    memcpy(mdls[top].name, name, strlen(name) + 1);
    mdls[top].vbo = model;
    mdls[top].vao = *vao;
    mdls[top].v_count = *v_count;
    top++;
    if (top > main_config.buf_vbo - 1) {
        top = 0;
        log_log(LOG_LOG, "VBO buffer end reached, back to 0", NULL);
    }
    return model;
}

GLuint bufs_tex(char * name)
{
    static size_t top = 0;
    FILE * file = NULL;
    uint32_t height = 0;
    uint32_t width = 0;
    GLuint tex = 0;

    for (size_t i = 0; i < main_config.buf_tex; i++) {
        if (strcmp(name, texs[i].name) == 0) {
            return texs[i].var;
        }
    }
    if (!open_file(&file, name, "rb")) {
        log_log(LOG_WARNING, "File \"%s\" does not exist, loading \"textures/missing.ktf\"", name);
        return mis_tex;
    }
    fread(&width, 4, 1, file);
    fread(&height, 4, 1, file);
    if (width > main_config.texsize_max ||
        height > main_config.texsize_max) {
        log_log(
            LOG_WARNING,
            "Texture \"%s\" is too big (W = %u/%u, H = %u/%u)",
            name, width, main_config.texsize_max, height,
            main_config.texsize_max
        );
        fclose(file);
        return mis_tex;
    }
    fread(tex_cont, 1, height * width * 3, file);
    fclose(file);
    tex = glf_load_tex(tex_cont, width, height);
    memcpy(texs[top].name, name, strlen(name) + 0);
    texs[top].var = tex;
    top++;
    if (top > main_config.buf_tex - 1) {
        top = 0;
        log_log(LOG_LOG, "Texture buffer end reached, back to 0", NULL);
    }
    return tex;
}

GLuint bufs_prog(char * vsname, char * fsname)
{
    static size_t top = 0;
    FILE * vs = NULL;
    FILE * fs = NULL;
    GLuint vert = 0;
    GLuint frag = 0;
    GLuint sp = 0;
    long l = 0;

    for (size_t i = 0; i < main_config.buf_prog; i++) {
        if (strcmp(prgs[i].vert, vsname) == 0 &&
            strcmp(prgs[i].frag, fsname) == 0) {
            return prgs[i].var;
        }
    }
    if (!open_file(&vs, vsname, "rb")) {
        log_log(LOG_WARNING, "File \"%s\" does not exist, loading missing shader program", vsname);
        return mis_sp;
    }
    fseek(vs, 0, SEEK_END);
    l = ftell(vs);
    rewind(vs);
    if (l > main_config.shader_chars_max - 1) {
        log_log(LOG_ERROR, "Vertex shader \"%s\" is too long", vsname);
        fclose(vs);
        return mis_sp;
    }
    fread(shd_cont, sizeof(char), l, vs);
    fclose(vs);
    shd_cont[l] = '\0';
    vert = glf_load_shader(shd_cont, GL_VERTEX_SHADER);
    if (!vert) {
        //TODO error handled
    }

    if (!open_file(&fs, fsname, "rb")) {
        log_log(LOG_WARNING, "File \"%s\" does not exist, loading missing shader program", fsname);
        return mis_sp;
    }
    fseek(fs, 0, SEEK_END);
    l = ftell(fs);
    rewind(fs);
    if (l > main_config.shader_chars_max - 1) {
        log_log(LOG_ERROR, "Fragment shader \"%s\" is too long", fsname);
        fclose(fs);
        glDeleteShader(vert);
        return mis_sp;
    }
    fread(shd_cont, 1, l, fs);
    fclose(fs);
    shd_cont[l] = '\0';
    frag = glf_load_shader(shd_cont, GL_FRAGMENT_SHADER);
    if (!frag) {
        //TODO error handled
    }

    sp = glf_load_shader_program(vert, frag);
    if (!sp) {
        //TODO error handler
    }
    memcpy(prgs[top].vert, vsname, strlen(vsname) + 1);
    memcpy(prgs[top].frag, fsname, strlen(fsname) + 1);
    prgs[top].var = sp;
    top++;
    if (top > main_config.buf_prog - 1) {
        top = 0;
        log_log(LOG_LOG, "Shader program buffer end reached, back to 0", NULL);
    }
    return sp;
}

//atstart()
void bufs_init(void)
{
    FILE * file = NULL;

    allocate_mem((void**)&lights, sizeof(Light), main_config.lights_max);
    allocate_mem((void**)&sectors, sizeof(Sector), main_config.sectors_max);
    allocate_mem((void**)&models, sizeof(Model), main_config.models_max);
    allocate_mem((void**)&prgs, sizeof(struct prg), main_config.buf_prog);
    allocate_mem((void**)&texs, sizeof(struct tex), main_config.buf_tex);
    allocate_mem((void**)&mdls, sizeof(struct mdl), main_config.buf_vbo);
    allocate_mem((void**)&mdl_cont, sizeof(float),
        main_config.vertices_max * VERTICES_ATTRIBS);
    allocate_mem((void**)&shd_cont, sizeof(char), main_config.shader_chars_max);
    allocate_mem((void**)&tex_cont, sizeof(uint8_t),
        main_config.texsize_max * main_config.texsize_max * 3);

    if (!open_file(&file, "textures/missing.ktf", "rb")) {
        log_log(LOG_ERROR, "Missing texture is missing", NULL);
        exit(EXIT_FAILURE);
    }
    uint32_t h = 0;
    uint32_t w = 0;
    fread(&w, 4, 1, file);
    fread(&h, 4, 1, file);
    if (w > 32 || h > 32) {
        log_log(LOG_WARNING, "Missing texture is too big (W = %u / %u, H = %u / %u)",
            w, 32, h, 32);
    }
    fread(tex_cont, 1, 32 * 32 * 3, file);
    fclose(file);
    mis_tex = glf_load_tex(tex_cont, 32, 32);

    if (!open_file(&file, "shaders/missing.vert", "r")) {
        log_log(LOG_ERROR, "Missing vertex shader is missing", NULL);
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);
    if (length > main_config.shader_chars_max - 1) {
        log_log(LOG_ERROR, "Missing vertex shader is too long (%u / %u)",
            length, main_config.shader_chars_max);
        exit(EXIT_FAILURE);
    }
    fread(shd_cont, 1, length, file);
    fclose(file);
    shd_cont[length] = '\0';
    mis_vs = glf_load_shader(shd_cont, GL_VERTEX_SHADER);
    if (!mis_vs)
        exit(EXIT_FAILURE);

    if (!open_file(&file, "shaders/missing.frag", "r")) {
        log_log(LOG_ERROR, "Missing fragment shader is missing", NULL);
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    rewind(file);
    if (length > main_config.shader_chars_max - 1) {
        log_log(LOG_ERROR, "Missing fragment shader is too long (%u / %u)",
            length, main_config.shader_chars_max);
        exit(EXIT_FAILURE);
    }
    fread(shd_cont, 1, length, file);
    fclose(file);
    shd_cont[length] = '\0';
    mis_fs = glf_load_shader(shd_cont, GL_FRAGMENT_SHADER);
    if (!mis_fs)
        exit(EXIT_FAILURE);

    mis_sp = glf_load_shader_program(mis_vs, mis_fs);
    if (!mis_sp)
        exit(EXIT_FAILURE);

    if (!open_file(&file, "models/missing.kmdl", "rb")) {
        log_log(LOG_ERROR, "Missing model is missing", NULL);
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    rewind(file);
    size_t elements = length / sizeof(float);
    if (elements > (main_config.vertices_max - 1) * VERTICES_ATTRIBS) {
        log_log(
            LOG_ERROR,
            "Missing model has too many vertices(%zu / %zu)",
            elements / VERTICES_ATTRIBS,
            main_config.vertices_max
        );
        elements = (main_config.vertices_max - 1) * VERTICES_ATTRIBS;
    }
    fread(mdl_cont, sizeof(float), elements, file);
    fclose(file);
    mis_mdl_vc = elements / VERTICES_ATTRIBS;
    glf_load_buffers(&mis_vao, &mis_vbo, mis_mdl_vc, mdl_cont);
}

//atexit() and when loading new scene
void bufs_clean(void)
{
    //TODO delete each object from gpu memory
}

//atexit()
void bufs_destroy(void)
{
    //TODO delete mis_*
}
