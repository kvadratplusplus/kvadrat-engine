#pragma once

#include <defines.h>

void camera_init(Camera *camera);
void camera_update(Camera *camera);
void framebuffer_size_callback(GLFWwindow * window, int width, int height);
