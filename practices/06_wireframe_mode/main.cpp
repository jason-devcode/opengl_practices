#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int width = 512;
int height = 512;
const char* title = "Test";

const char* vs_src = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;

void main() {
  vColor = vec3( aColor );
  gl_Position = vec4(aPos, 1.0);
}
)";

const char* fs_src = R"(
#version 330 core

in vec3 vColor;
out vec4 FragColor;

void main() {
  FragColor = vec4(vColor, 1.0);
}
)";

void process_inputs( GLFWwindow* );

typedef struct {
  GLuint program;
  GLint success;
} ShaderResult;


bool checkShaderCompileStatus( GLuint shader_object, const char* shader_type ) {
  GLint success = GL_FALSE;
  glGetShaderiv( shader_object, GL_COMPILE_STATUS, &success );

  if( not success ) {
    char info_log[ 1024 ];
    glGetShaderInfoLog( shader_object, 1024, NULL, info_log );
    std::cerr << "Error: Failed to compile" << shader_type << "!\n" << "Log: \n" << info_log << "\n";
    return false;
  }

  return true;
}

ShaderResult process_shaders() {
  // Compile vertex shader
  GLuint vs = glCreateShader( GL_VERTEX_SHADER ); 
  glShaderSource( vs, 1, &vs_src, NULL );
  glCompileShader( vs );

  if( not checkShaderCompileStatus( vs, "vertex shader" ) )
    return (ShaderResult){ 0, GL_FALSE };

  // Compile fragment shader
  GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
  glShaderSource( fs, 1, &fs_src, NULL );
  glCompileShader( fs );

  if( not checkShaderCompileStatus( fs, "fragment shader" ) )
    return (ShaderResult){ 0, GL_FALSE };

  // Create and link shader program
  GLuint program = glCreateProgram();
  glAttachShader( program, vs );
  glAttachShader( program, fs );
  glLinkProgram( program );

  // Free shader sources
  glDeleteShader( vs );
  glDeleteShader( fs );

  return (ShaderResult){
    program,
    GL_TRUE
  };
}

typedef struct {
  struct { float x, y, z; } pos;
  struct { float r, g, b; } color;
} Vertex;

typedef struct {
  GLuint A, B, C;
} Triangle;

#define INDEX_PER_TRIANGLE (sizeof( Triangle ) / sizeof( GLuint ))

GLuint build_vao() {
  const unsigned int pos_attrib = 0;
  const unsigned int color_attrib = 1;

  Vertex vertices[] = {
    { .pos = { .x = -0.5f, .y =  0.5f, .z = 0.0f }, .color = { .r = 1.0f, .g = 0.0f, .b = 0.0f  } },
    { .pos = { .x =  0.5f, .y =  0.5f, .z = 0.0f }, .color = { .r = 0.0f, .g = 1.0f, .b = 0.0f  } },
    { .pos = { .x =  0.5f, .y = -0.5f, .z = 0.0f }, .color = { .r = 0.0f, .g = 0.0f, .b = 1.0f  } },
    { .pos = { .x = -0.5f, .y = -0.5f, .z = 0.0f }, .color = { .r = 1.0f, .g = 1.0f, .b = 1.0f  } }
  };

  Triangle indices[] = {
    { .A = 0, .B = 1, .C = 3},
    { .A = 3, .B = 1, .C = 2}
  };

  // Create VBO and submit data to GPU buffer
  GLuint VBO;
  glGenBuffers( 1, &VBO );
  glBindBuffer( GL_ARRAY_BUFFER, VBO );
  glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

  // Create VAO
  GLuint VAO;
  glGenVertexArrays( 1, &VAO );
  glBindVertexArray( VAO );

  // Create EBO/Index-Buffer and bind to current VAO
  GLuint EBO;
  glGenBuffers( 1, &EBO );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW );

  // Store attribute pointers
  // Vertex Position 
  glVertexAttribPointer( pos_attrib, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)0 );
  glEnableVertexAttribArray( pos_attrib );

  // Vertex Color
  glVertexAttribPointer( color_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (sizeof(float) * 3) );
  glEnableVertexAttribArray( color_attrib );

  // Unbind VAO and buffers
  glBindVertexArray( 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  return VAO;
}

void render_loop( GLFWwindow* window ) {
  glViewport( 0, 0, width, height );
  glClearColor( 0, 0, 0, 1 );

  ShaderResult shader = process_shaders();
  if( not shader.success ) return;

  GLuint vao = build_vao();

  while ( not glfwWindowShouldClose( window ) ) {
    process_inputs( window );
    glClear( GL_COLOR_BUFFER_BIT );
    glUseProgram( shader.program );
    // Call draw to draw rectangle VAO
    glBindVertexArray( vao );
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
    glfwSwapBuffers( window );
    glfwPollEvents();
  }
}

void process_inputs( GLFWwindow* window ) {
  if( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
    glfwSetWindowShouldClose( window, true );

  // Press 1 for change to wireframe draw mode
  if( glfwGetKey( window, GLFW_KEY_1 ) == GLFW_PRESS )
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  // Press 2 for change to fill draw mode
  if( glfwGetKey( window, GLFW_KEY_2 ) == GLFW_PRESS )
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
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
