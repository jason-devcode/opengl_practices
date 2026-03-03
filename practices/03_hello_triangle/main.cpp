#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int width = 512;
int height = 512;

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;

void main() {
  vColor = vec3(aColor);
  gl_Position = vec4( aPos.x, aPos.y, aPos.z, 1.0 );
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

in vec3 vColor;
out vec4 FragColor;

void main() {
  FragColor = vec4(vColor,1.0);
}
)";


void processInput( GLFWwindow* window ) {
  if( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
    glfwSetWindowShouldClose(window, true);
  }
}

int main() {
  glfwInit();

  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

  GLFWwindow* window = glfwCreateWindow( width, height, "Hello Triangle!", NULL, NULL );

  if( window == NULL ) {
    std::cerr << "Failed to create window\n";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if( not gladLoadGLLoader((GLADloadproc) glfwGetProcAddress ) ) {
    std::cerr << "Failed to load GL loader\n";
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }
  
  // 1. Compile Vertex Shader
  GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
  glShaderSource( vertexShader, 1, &vertexShaderSource, NULL );
  glCompileShader(vertexShader);

  // 2. Compile Fragment Shader
  GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
  glShaderSource( fragmentShader, 1, &fragmentShaderSource, NULL );
  glCompileShader(fragmentShader);

  // 3. Create shader program and link shaders
  GLuint shaderProgram = glCreateProgram();
  glAttachShader( shaderProgram, vertexShader );
  glAttachShader( shaderProgram, fragmentShader );
  glLinkProgram( shaderProgram );

  //4. Clean unused resources
  glDeleteShader( vertexShader );
  glDeleteShader( fragmentShader );

  // Triangle vertices raw data
  GLfloat vertices[] = {
  //  x      y     z     r     g     b
     0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
  };

  // 1. Vertex Buffer Object to send vertex data to VRAM
  GLuint VBO;
  glGenBuffers(1, &VBO);

  // 2. Create VAO
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Bind VBO to ARRAY BUFFER
  // After bind VBO each state-setting operation performed over ARRAY BUFFER affects to VBO
  glBindBuffer( GL_ARRAY_BUFFER, VBO );

  // Copy vertices data to VBO
  glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

  // XYZ
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, 0 );
  glEnableVertexAttribArray(0);
  // RGB
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (void*)(sizeof(GLfloat) * 3) );
  glEnableVertexAttribArray(1);
  
  glViewport( 0, 0, width, height );
  glClearColor( 0, 0, 0, 1 );

  while( not glfwWindowShouldClose( window ) ) {
    processInput(window);

    glClear( GL_COLOR_BUFFER_BIT );
    glUseProgram(shaderProgram);
    glDrawArrays( GL_TRIANGLES, 0, 3 );
    
    glfwSwapBuffers( window );
    glfwPollEvents();
  }

  // Delete VBO data and unbind from GL_ARRAY_BUFFER target
  glDeleteBuffers( 1, &VBO );

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}


