#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include <defines.h>
#include <log_man.h>

#define POINTERS_MAX 128

static void * pointers[POINTERS_MAX];

static inline void free_at_exit(void * ptr)
{
    static uint16_t stack_top = 0;
    if (stack_top > POINTERS_MAX - 1) {
        log_log(LOG_ERROR, "Too many resources allocated", NULL);
        exit(EXIT_FAILURE);
    }
    pointers[stack_top++] = ptr;
}

//allocate_mem((void**)&ptr, sizeof(type), TYPES_MAX);
void allocate_mem(void ** ptr, size_t size, size_t number)
{
    *ptr = calloc(number, size);
    if (*ptr == NULL) {
        log_log(LOG_ERROR, "Failed to allocate memory (%zu bytes, %f kb)", size * number, (float)(size * number) / 1024);
        exit(EXIT_FAILURE);
    }
    free_at_exit(*ptr);
}

//registered in atexit()
void free_all_recources(void)
{
    for (size_t i = 0; i < POINTERS_MAX; i++)
        if (pointers[i])
            free(pointers[i]);

    glfwTerminate();
}
