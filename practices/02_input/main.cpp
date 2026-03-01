#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int width = 512;
int height = 512;

void processInput( GLFWwindow* window ) {
    if( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
        glfwSetWindowShouldClose( window, true );
    }
}

int main() {
    glfwInit();

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    GLFWwindow* window = glfwCreateWindow( width, height, "02 - Input Example", NULL, NULL );

    if( window == NULL ) {
        std::cerr << "Failed to create window!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent( window );

    if( not gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress ) ) {
        std::cerr << "Failed to load GL loader!\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;

    }

    glViewport(0,0,width,height);
    glClearColor(0,0,1,1);

    while (not glfwWindowShouldClose(window))
    {
        processInput(window);

        glClear( GL_COLOR_BUFFER_BIT );
        glfwSwapBuffers(window);

        
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

