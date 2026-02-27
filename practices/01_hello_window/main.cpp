#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int width = 512;
int height = 512;

int main() {
  glfwInit();

  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

  GLFWwindow* window = glfwCreateWindow( width, height, "Hello Window!", NULL, NULL );

  if( window == NULL ) {
    std::cerr << "Failed to create window\n";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if( not gladLoadGLLoader((GLADloadproc) glfwGetProcAddress ) ) {
    std::cerr << "Failed to load GL loader\n";
    glfwTerminate();
    return -1;
  }

  glViewport( 0, 0, width, height );
  glClearColor( 0, 0, 1, 1 );
  glClear( GL_COLOR_BUFFER_BIT );
  glfwSwapBuffers( window );

  while( not glfwWindowShouldClose( window ) ) {
    glfwPollEvents();
  }


  glfwTerminate();
  return 0;
}


