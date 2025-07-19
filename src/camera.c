//TODO move to player.c

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <defines.h>
#include <kmatrix.h>

extern Camera main_camera;
extern Config main_config;

void camera_init(Camera * camera)
{
    camera->camera_position.x = 0;
    camera->camera_position.y = 0;
    camera->camera_position.z = 3;

    camera->camera_front.x = 0;
    camera->camera_front.y = 0;
    camera->camera_front.z = -1;

    camera->camera_up.x = 0;
    camera->camera_up.y = 1;
    camera->camera_up.z = 0;

    camera->view_matrix = kmat_identity();
}

void camera_update(Camera * camera)
{
    kvec3 camera_target;
    
    camera_target = kvec_add(&camera->camera_position, &camera->camera_front);
    camera->view_matrix = kmat_lookat(&camera->camera_position, &camera_target, &camera->camera_up);
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    main_config.screen_height = height;
    main_config.screen_width = width;
    main_camera.projection_matrix = kmat_perspective(main_config.screen_width, main_config.screen_height, 120, 100, 0.1);
    
    glViewport(0, 0, width, height);
}
