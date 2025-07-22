#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#ifdef __WIN32
#include <windows.h>
#endif
#ifdef __linux__
#include <linux/limits.h>
#endif

#include <memory_man.h>
#include <defines.h>
#include <input.h>
#include <camera.h>
#include <scene.h>
#include <log_man.h>
#include <file_man.h>
#include <config.h>
#include <sector.h>
#include <glfuncs.h>

kTime main_time;
Camera main_camera;
DirLight dir_light;
Config main_config;

Light * lights;
Sector * sectors;
Model * models;
char * cwd;

int main(int argc, const char ** argv)
{
    printf("Engine "ENGINE_VERSION"\n");
    
    atexit(free_all_recources);
    
    allocate_mem((void**)&cwd, sizeof(char), PATH_MAX + 1);
    extract_directory(argv[0], cwd, PATH_MAX);

    config_read("config.cfg");
    config_read_args(argv, argc);
    read_bindings("binds.txt");     //TODO move to player_init
    
    //opengl 3.3
    if (!glfwInit()) {
        log_log(LOG_ERROR, "Failed to initialize GLFW\n", NULL);
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow * window = NULL;
    if (main_config.fullscreen) {
        const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        log_log(LOG_LOG, "Monitor resolution: %d/%d, Config resolution: %d/%d",
            mode->width, mode->height, main_config.screen_width, main_config.screen_height);
        window = glfwCreateWindow(mode->width, mode->height, "kvadrat engine 2",
            glfwGetPrimaryMonitor(), NULL);
        main_config.screen_height = mode->height;
        main_config.screen_width = mode->width;
    } else {
        window = glfwCreateWindow(main_config.screen_width, main_config.screen_height,
            "kvadrat engine 2", NULL, NULL);
    }
    if (!window) {
        log_log(LOG_ERROR, "Failed to create window", NULL);
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        log_log(LOG_ERROR, "Failed to initialize GLAD", NULL);
        exit(EXIT_FAILURE);
    }
    //callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //options
    if (main_config.unlim)
        glfwSwapInterval(0);

    stbi_set_flip_vertically_on_load(1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glClearColor(0, 0, 0, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glf_precompile_missing();
    framebuffer_size_callback(window, main_config.screen_width, main_config.screen_height);
    camera_init(&main_camera);
    
    allocate_mem((void**)&lights, sizeof(Light), main_config.lights_max);
    allocate_mem((void**)&sectors, sizeof(Sector), main_config.sectors_max);
    allocate_mem((void**)&models, sizeof(Model), main_config.models_max);

    scene_load_scene("scenes/example.ksc");

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static float last_frame;
        float current_frame = glfwGetTime();
        main_time.time = current_frame;
        main_time.delta_time = current_frame - last_frame;
        main_time.sin_time = sinf(main_time.time);
        main_time.cos_time = cosf(main_time.time);
        last_frame = current_frame;

        if (main_config.fps)
            printf("%f\n", 1 / main_time.delta_time);

        for (size_t i = 0; i < main_config.sectors_max; i++) {
            if (sectors[i].is_active == 0)
                continue;

            sector_draw(&sectors[i]);
        }
        for (size_t i = 0; i < main_config.models_max; i++) {
            if (models[i].is_active == 0)
                continue;

            size_t indeces[main_config.shader_lights_count];
            kvec3 pos = {models[i].model_matrix.x4, models[i].model_matrix.y4,
                models[i].model_matrix.z4};

            glf_find_nearest_lights(&pos, indeces);
            glf_load_light_pos(indeces, models[i].shader_program);
            glf_draw(models[i].shader_program, models[i].texture,
                models[i].vao, &models[i].model_matrix, 0, models[i].vertex_count);
        }
        process_input(window);
        camera_update(&main_camera);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    exit(EXIT_SUCCESS);
}
