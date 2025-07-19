//NOT READY YET

#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <kmatrix.h>

typedef struct
{
    kvec2 points[4];
    float top;
    float bottom;

    uint8_t portal[4];
    float connection_top[4];
    float connection_bottom[4];

    char texture_names[6][128];
    char vertex_shader_names[6][128];
    char fragment_shader_names[6][128];
    kvec2 tex_coords[6];

    //editor only
    uint8_t is_active;
    GLuint vao, vbo;
} SectorLayout;

kmat4 projection_matrix;
kmat4 view_matrix;
GLuint shader_program;
float delta_time;
kvec3 eye = {0, 0, 1};
kvec3 target = {0, 0, 0};
kvec3 up = {0, 1, 0};
SectorLayout * sectors;
size_t sectors_max = 0;

void framebuffer_size_callback(GLFWwindow * window, int width, int height);
void process_input(GLFWwindow * window);
//GLuint glf_load_shaders(void);

GLuint glf_load_shaders(void)
{
    const char * vs =
    "#version 330 core\n"
    "layout (location = 0) in vec2 i_pos;"
    "uniform mat4 view;"
    "uniform mat4 projection;"
    "void main()"
    "{"
    "gl_Position = projection * view * vec4(i_pos, 0.0, 1.0);"
    "}"
    ;
    const char * fs =
    "#version 330 core\n"
    "out vec4 frag_color;"
    "void main()"
    "{"
    "frag_color = vec4(1.0, 1.0, 1.0, 1.0);"
    "}"
    ;

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vs, NULL);
    glCompileShader(vshader);

    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fs, NULL);
    glCompileShader(fshader);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vshader);
    glAttachShader(shader_program, fshader);
    glLinkProgram(shader_program);

    return shader_program;
}

void glf_load_buffers(unsigned int * vao, unsigned int * vbo, float * vertex_array)
{
    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 2, vertex_array, GL_DYNAMIC_DRAW);

    //position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    //unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}

void glf_draw(GLuint vao)
{
    glUseProgram(shader_program);

    //matrices
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, &view_matrix.x1);
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, &projection_matrix.x1);

    //draw
    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

int main(int argc, char const *argv[])
{
    FILE * config = fopen("config.cfg", "r");
    if (!config)
    {
        fprintf(stderr, "Error: \"config.cfg\" file is not exists\n");
        exit(EXIT_FAILURE);
    }
    
    {
        char str[32];
        while (fscanf(config, "%31s", str) != EOF)
        {
            if (strcmp(str, "sectors_max") == 0)
            {
                if (fscanf(config, "%zu", &sectors_max) != 1)
                {
                    fprintf(stderr, "Error: \"sectors_max\" in \"config.cfg\" is invalid\n");
                    exit(EXIT_FAILURE);
                }

                else
                {
                    break;
                }
            }
        }

        if (sectors_max == 0)
        {
            fprintf(stderr, "Error: \"sectors_max\" not found in \"config.cfg\"\n");
            exit(EXIT_FAILURE);
        }
    }

    

    //opengl 3.3
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow * window = NULL;
    window = glfwCreateWindow(1280, 720, "levelbuilder", NULL, NULL);

    if (!window)
    {
        fprintf(stderr, "Failed to create window\n");
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
        exit(EXIT_FAILURE);
    }

    sectors = (SectorLayout *)calloc(sectors_max, sizeof(SectorLayout));

    //callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetKeyCallback(window, key_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetScrollCallback(window, scroll_callback);
    framebuffer_size_callback(window, 1280, 720);
    glClearColor(0, 0, 0, 1.0f);

    GLuint vbo, vao;
    float vertices[] = {-1, -1, 0, 1, 1, -1, 0, 0};
    shader_program = glf_load_shaders();
    glf_load_buffers(&vao, &vbo, vertices);
    
    view_matrix = kmat_lookat(&eye, &target, &up);

    while (!glfwWindowShouldClose(window))
    {
        static float last_frame;
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        process_input(window);

        glClear(GL_COLOR_BUFFER_BIT);
        glf_draw(vao);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    projection_matrix = kmat_perspective(width, height, 90, 100, 0.1);
    glViewport(0, 0, width, height);
}

void termianl_input(void)
{
    char str[1024];

    scanf("%s");

    if (strcmp(str, "create") == 0)
    {
        printf("Enter 4 points (counter clockwise) [float x, float y]")
        scanf("%f %f %f %f %f %f %f %f", );
    }
}

void process_input(GLFWwindow * window)
{
    float camera_speed = 1 * delta_time;

    if (glfwGetKey(window, GLFW_KEY_W))
    {
        eye.y += camera_speed;
        target.y += camera_speed;
    }

    if (glfwGetKey(window, GLFW_KEY_S))
    {
        eye.y -= camera_speed;
        target.y -= camera_speed;
    }

    if (glfwGetKey(window, GLFW_KEY_D))
    {
        eye.x += camera_speed;
        target.x += camera_speed;
    }

    if (glfwGetKey(window, GLFW_KEY_A))
    {
        eye.x -= camera_speed;
        target.x -= camera_speed;
    }

    if (glfwGetKey(window, GLFW_KEY_APOSTROPHE))
    {
        termianl_input();
    }

    view_matrix = kmat_lookat(&eye, &target, &up);
}
