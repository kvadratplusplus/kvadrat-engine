#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <input.h>
#include <defines.h>
#include <file_man.h>
#include <log_man.h>

extern Camera main_camera;
extern kTime main_time;

typedef struct
{
    //move
    int move_fwd;   //forward
    int move_bck;   //backward
    int move_rgh;   //right
    int move_lft;   //left

    //wireframe
    int wireframe_enable;
    int wireframe_disable;
    int window_close;

    //cursor
    int cursor_unlock;
    int cursor_lock;
} KeyBindings;

typedef struct
{
    char * name;
    int key_code;
} Key;

static KeyBindings key_bindings;

//functions starts at line 172
static Key keys[] =
{
    //1st row
    { "esc", GLFW_KEY_ESCAPE },
    { "f1", GLFW_KEY_F1 },
    { "f2", GLFW_KEY_F2 },
    { "f3", GLFW_KEY_F3 },
    { "f4", GLFW_KEY_F4 },
    { "f5", GLFW_KEY_F5 },
    { "f6", GLFW_KEY_F6 },
    { "f7", GLFW_KEY_F7 },
    { "f8", GLFW_KEY_F8 },
    { "f9", GLFW_KEY_F9 },
    { "f10", GLFW_KEY_F10 },
    { "f11", GLFW_KEY_F11 },
    { "f12", GLFW_KEY_F12 },

    //2nd row
    { "~", GLFW_KEY_GRAVE_ACCENT },
    { "1", GLFW_KEY_1 },
    { "2", GLFW_KEY_2 },
    { "3", GLFW_KEY_3 },
    { "4", GLFW_KEY_4 },
    { "5", GLFW_KEY_5 },
    { "6", GLFW_KEY_6 },
    { "7", GLFW_KEY_7 },
    { "8", GLFW_KEY_8 },
    { "9", GLFW_KEY_9 },
    { "0", GLFW_KEY_0 },
    { "-", GLFW_KEY_MINUS },
    { "=", GLFW_KEY_EQUAL },
    { "\\", GLFW_KEY_BACKSLASH },
    { "backspace", GLFW_KEY_BACKSPACE},

    //3rd row
    { "tab", GLFW_KEY_TAB },
    { "q", GLFW_KEY_Q },
    { "w", GLFW_KEY_W },
    { "e", GLFW_KEY_E },
    { "r", GLFW_KEY_R },
    { "t", GLFW_KEY_T },
    { "y", GLFW_KEY_Y },
    { "u", GLFW_KEY_U },
    { "i", GLFW_KEY_I },
    { "o", GLFW_KEY_O },
    { "p", GLFW_KEY_P },
    { "[", GLFW_KEY_LEFT_BRACKET },
    { "]", GLFW_KEY_RIGHT_BRACKET },        //Ъ

    //4th row
    { "caps", GLFW_KEY_CAPS_LOCK },
    { "a", GLFW_KEY_A },
    { "s", GLFW_KEY_S },
    { "d", GLFW_KEY_D },
    { "f", GLFW_KEY_F },
    { "g", GLFW_KEY_G },
    { "h", GLFW_KEY_H },
    { "j", GLFW_KEY_J },
    { "k", GLFW_KEY_K },
    { "l", GLFW_KEY_L },
    { ";", GLFW_KEY_SEMICOLON },
    { "\'", GLFW_KEY_APOSTROPHE },
    { "enter", GLFW_KEY_ENTER },

    //5th row
    { "lshift", GLFW_KEY_LEFT_SHIFT },
    { "z", GLFW_KEY_Z },
    { "x", GLFW_KEY_X },
    { "c", GLFW_KEY_C },
    { "v", GLFW_KEY_V },
    { "b", GLFW_KEY_B },
    { "n", GLFW_KEY_N },
    { "m", GLFW_KEY_M },
    { ",", GLFW_KEY_COMMA },
    { ".", GLFW_KEY_PERIOD },
    { "/", GLFW_KEY_SLASH },
    { "rshift", GLFW_KEY_RIGHT_SHIFT },
    
    //6th row
    { "lctrl", GLFW_KEY_LEFT_CONTROL },
    { "lsuper", GLFW_KEY_LEFT_SUPER },
    { "lalt", GLFW_KEY_LEFT_ALT },
    { "space", GLFW_KEY_SPACE },
    { "ralt", GLFW_KEY_RIGHT_ALT },
    { "rsuper", GLFW_KEY_RIGHT_SUPER },
    { "strange_button", GLFW_KEY_MENU },       //https://i.sstatic.net/kF3jU.png
    { "rctrl", GLFW_KEY_RIGHT_CONTROL },

    //text movement
    { "insert", GLFW_KEY_INSERT },
    { "home", GLFW_KEY_HOME },
    { "page_up", GLFW_KEY_PAGE_UP },
    { "delete", GLFW_KEY_DELETE },
    { "end", GLFW_KEY_END },
    { "page_down", GLFW_KEY_PAGE_DOWN },

    //weird buttons
    { "print_screen", GLFW_KEY_PRINT_SCREEN },      //idk who will use these three weird buttons
    { "scroll_lock", GLFW_KEY_SCROLL_LOCK },
    { "pause_break", GLFW_KEY_PAUSE },

    //arrows
    { "up", GLFW_KEY_UP },
    { "left", GLFW_KEY_LEFT },
    { "down", GLFW_KEY_DOWN },
    { "right", GLFW_KEY_RIGHT },
    
    //keypad
    { "num_lock", GLFW_KEY_NUM_LOCK },
    { "kp_/", GLFW_KEY_KP_DIVIDE },
    { "kp_*", GLFW_KEY_KP_MULTIPLY },
    { "kp_-", GLFW_KEY_KP_SUBTRACT },
    { "kp_7", GLFW_KEY_KP_7 },
    { "kp_8", GLFW_KEY_KP_8 },
    { "kp_9", GLFW_KEY_KP_9 },
    { "kp_+", GLFW_KEY_KP_ADD },
    { "kp_4", GLFW_KEY_KP_4 },
    { "kp_5", GLFW_KEY_KP_5 },
    { "kp_6", GLFW_KEY_KP_6 },
    { "kp_1", GLFW_KEY_KP_1 },
    { "kp_2", GLFW_KEY_KP_2 },
    { "kp_3", GLFW_KEY_KP_3 },
    { "kp_enter", GLFW_KEY_KP_ENTER },
    { "kp_0", GLFW_KEY_KP_0 },
    { "kp_.", GLFW_KEY_KP_DECIMAL }
};

static inline int recognize_button(char * button_name)
{
    for (size_t i = 0; i < sizeof(keys) / sizeof(Key); i++)
        if (strcmp(button_name, keys[i].name) == 0)
            return keys[i].key_code;

    log_log(LOG_WARNING, "Unknown button name \"%s\"", button_name);
    return GLFW_KEY_UNKNOWN;
}

void read_bindings(char * bindings_file_name)
{
    FILE * bindings_file = NULL;
    if(!open_file(&bindings_file, bindings_file_name, "r")) {
        log_log(LOG_ERROR, "Bindings file does not exist", NULL);
        exit(EXIT_FAILURE);
    }
    char binding_name[32];      //move_fwd, move_bck, etc
    char button_name[16];       //w, s, f11, etc
    while (fscanf(bindings_file, "%31s %15s", binding_name, button_name) != EOF) {
        int button = recognize_button(button_name);

        if (strcmp(binding_name, "wireframe_enable") == 0)
            key_bindings.wireframe_enable = button;

        else if (strcmp(binding_name, "wireframe_disable") == 0)
            key_bindings.wireframe_disable = button;

        else if (strcmp(binding_name, "window_close") == 0)
            key_bindings.window_close = button;

        else if (strcmp(binding_name, "move_fwd") == 0)
            key_bindings.move_fwd = button;

        else if (strcmp(binding_name, "move_rgh") == 0)
            key_bindings.move_rgh = button;

        else if (strcmp(binding_name, "move_bck") == 0)
            key_bindings.move_bck = button;

        else if (strcmp(binding_name, "move_lft") == 0)
            key_bindings.move_lft = button;

        else if (strcmp(binding_name, "cursor_lock") == 0)
            key_bindings.cursor_lock = button;

        else if (strcmp(binding_name, "cursor_unlock") == 0)
            key_bindings.cursor_unlock = button;

        else
            log_log(LOG_WARNING, "Unknown binding name \"%s\"", binding_name);
    }
    fclose(bindings_file);
}

//TODO convert process_input and key_callback to what is in Source
//+move_forward -move_forward, etc
//pressing the "w" key = +move_fwd
//releasing the "w" key = -move_fwd
//and handling the move in player_move
//if (move_fwd){ ...
void process_input(GLFWwindow * window)
{
    float camera_speed = 10 * main_time.delta_time;

    if (glfwGetKey(window, key_bindings.move_fwd)) {
        kvec3 temp;

        temp = kvec_scale(&main_camera.camera_front, camera_speed);
        main_camera.camera_position = kvec_add(&main_camera.camera_position, &temp);
    }
    if (glfwGetKey(window, key_bindings.move_bck)) {
        kvec3 temp;

        temp = kvec_scale(&main_camera.camera_front, camera_speed);
        main_camera.camera_position = kvec_sub(&main_camera.camera_position, &temp);
    }
    if (glfwGetKey(window, key_bindings.move_rgh)) {
        kvec3 temp;

        temp = kvec_cross(&main_camera.camera_front, &main_camera.camera_up);
        temp = kvec_normalize(&temp);
        temp = kvec_scale(&temp, camera_speed);
        main_camera.camera_position = kvec_add(&main_camera.camera_position, &temp);
    }
    if (glfwGetKey(window, key_bindings.move_lft)) {
        kvec3 temp;

        temp = kvec_cross(&main_camera.camera_front, &main_camera.camera_up);
        temp = kvec_normalize(&temp);
        temp = kvec_scale(&temp, camera_speed);
        main_camera.camera_position = kvec_sub(&main_camera.camera_position, &temp);
    }
}

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key == key_bindings.window_close)
        glfwSetWindowShouldClose(window, 1);

    if (action == GLFW_PRESS && key == key_bindings.wireframe_enable)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (action == GLFW_PRESS && key == key_bindings.wireframe_disable)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (action == GLFW_PRESS && key == key_bindings.cursor_lock)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (action == GLFW_PRESS && key == key_bindings.cursor_unlock)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
    static float last_x = 0;
    static float last_y = 0;
    static uint8_t firstMouse = 1;

    if (firstMouse) {
        firstMouse = 0;
        last_x = xpos;
        last_y = ypos;
    }
    float x_offset = xpos - last_x;
    float y_offset = last_y - ypos;
    last_x = xpos;
    last_y = ypos;

    float sensitivity = 0.1;

    y_offset *= sensitivity;
    x_offset *= sensitivity;

    main_camera.camera_yaw += x_offset;
    main_camera.camera_pitch += y_offset;

    if (main_camera.camera_pitch > 89.0f)
        main_camera.camera_pitch =  89.0f;

    if (main_camera.camera_pitch < -89.0f)
        main_camera.camera_pitch = -89.0f;

    kvec3 dir;

    dir.x = cos(deg_to_rad(main_camera.camera_yaw)) * cos(deg_to_rad(main_camera.camera_pitch));
    dir.y = sin(deg_to_rad(main_camera.camera_pitch));
    dir.z = sin(deg_to_rad(main_camera.camera_yaw)) * cos(deg_to_rad(main_camera.camera_pitch));

    dir = kvec_normalize(&dir);

    main_camera.camera_front.x = dir.x;
    main_camera.camera_front.y = dir.y;
    main_camera.camera_front.z = dir.z;
}


void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
    //use yoffset
}
