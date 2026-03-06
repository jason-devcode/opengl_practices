#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void processInput_(GLFWwindow *window);

#define processInput() processInput_(window)

int width = 512;
int height = 512;

#define render()             \
    glfwSwapBuffers(window); \
    glfwPollEvents();

#define gameloop \
    while (not glfwWindowShouldClose(window))

const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    out vec3 vColor;

    void main() {
        vColor = vec3(aColor);
        gl_Position = vec4( aPos.x, aPos.y, aPos.z, 1.0 );
    }
)";

const char *fragmentShaderSource = R"(
    #version 330 core

    in vec3 vColor;
    out vec4 FragColor;

    void main() {
        FragColor = vec4(vColor,1.0);
    }
)";

typedef struct {
    GLuint shaderProgram;
    GLint status; // GL_FALSE -> Failed to compile shaders, GL_TRUE -> Success
} ShaderResult;



ShaderResult process_shaders()
{
    ShaderResult result;
    const size_t info_size = 1024;
    char info_log[info_size];

    // 1. Compile Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check Compilation status
    glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &result.status );
    if( not result.status ) {
        glGetShaderInfoLog( vertexShader, info_size, NULL, info_log );
        std::cerr << "Error: Failed to compile vertex shader!\n" << "Log: \n" << info_log << "\n";
        return result;
    }

    // 2. Compile Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);


    // Check Compilation status
    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &result.status );
    if( not result.status ) {
        glGetShaderInfoLog( fragmentShader, info_size, NULL, info_log );
        std::cerr << "Error: Failed to compile fragment shader!\n" << "Log: \n" << info_log << "\n";
        return result;
    }

    // 3. Create shader program and link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // 4. Clean unused resources
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    result.shaderProgram = shaderProgram;
    result.status = GL_TRUE;

    return result;
}

GLuint build_vao()
{
    // Triangle vertices raw data
    GLfloat vertices[] = {
        //  x      y     z     r     g     b
          -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
           0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
           0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
          -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f
        };

    // Indices
    GLuint indices[] = {
        3, 0, 1,
        3, 1, 2
    };

    // 1. Vertex Buffer Object to send vertex data to VRAM
    GLuint VBO;
    glGenBuffers(1, &VBO);

    // Copy vertices data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 2. Create VAO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // XYZ
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, 0);
    glEnableVertexAttribArray(0);
    // RGB
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (void *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);

    // 3. Create EBO
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return VAO;
}

void render_loop(GLFWwindow *window)
{
    ShaderResult shader = process_shaders();
    if( not shader.status ) return;

    GLuint vao = build_vao();

    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 1);
    glBindVertexArray(vao);

    gameloop
    {
        processInput();
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader.shaderProgram);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        render();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shader.shaderProgram);
}

/////////////////////////////////////////////////////////////
/// WINDOW MANAGEMENT FUNCTIONS
/////////////////////////////////////////////////////////////

void processInput_(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow *, int w, int h)
{
    glViewport(0, 0, w, h);
}

GLFWwindow *init_engine()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(width, height, "Index-Buffer / EBO", NULL, NULL);

    if (window == NULL)
    {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (not gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to load GL loader\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return NULL;
    }

    return window;
}

void close_engine(GLFWwindow *window)
{
    if (window)
        glfwDestroyWindow(window);
    glfwTerminate();
}

int main()
{
    GLFWwindow *window = init_engine();
    if (window == NULL)
        return -1;

    render_loop(window);
    close_engine(window);
    return 0;
}
