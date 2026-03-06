#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int width = 512;
int height = 512;
const char* title = "Test";

void process_inputs( GLFWwindow* );

void render_loop( GLFWwindow* window ) {
  glViewport( 0, 0, width, height );
  glClearColor( 0, 0, 0, 1 );

  int nAttributes;
  glGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &nAttributes );

  std::cout << "MAX VERTEX ATTRIBUTES SUPPORTED: " << nAttributes << "\n";

  while ( not glfwWindowShouldClose( window ) ) {
    process_inputs( window );
    glClear( GL_COLOR_BUFFER_BIT );
    glfwSwapBuffers( window );
    glfwPollEvents();
  }
}

void process_inputs( GLFWwindow* window ) {
  if( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
    glfwSetWindowShouldClose( window, true );
}

void framebuffer_size_callback( GLFWwindow* , int w, int h ) {
  glViewport( 0, 0, w, h );
}

GLFWwindow* init_engine() {
  glfwInit();

  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

  GLFWwindow* window = glfwCreateWindow( width, height, title, NULL, NULL );

  if( window == NULL ) {
    std::cerr << "Failed to create window\n";
    glfwTerminate();
    return NULL;
  }

  glfwMakeContextCurrent( window );
  glfwSetFramebufferSizeCallback( window, framebuffer_size_callback );

  if( not gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress ) ) {
    std::cerr << "Failed to load GL loader \n";
    glfwDestroyWindow( window );
    glfwTerminate();
    return NULL;
  }

  return window;
}

void close_engine( GLFWwindow* window ) {
  if( window ) glfwDestroyWindow( window );
  glfwTerminate();
}

int main() {
  GLFWwindow* window = init_engine();
  if( not window ) return -1;
  render_loop( window );
  close_engine( window );
  return 0;
}
