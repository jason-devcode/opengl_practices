#define LOG_USE_COLOR

#include "libs/engine.h"
#include "libs/shader.h"
#include "libs/texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdint.h>
#include <string.h>

#define WIDTH 512 * 2
#define HEIGHT 512 * 2
#define TITLE "16 - 3D Projection"

using vec3 = glm::vec3;
using vec2 = glm::vec2;

typedef struct
{
  vec3 pos;
  vec3 color;
  vec2 tx;
} Vertex;

typedef struct
{
  GLuint A, B, C;
} Triangle;

#define INDEX_PER_TRIANGLE (sizeof(Triangle) / sizeof(GLuint))

GLuint build_vao()
{
  const unsigned int pos_attrib = 0;
  const unsigned int color_attrib = 1;
  const unsigned int tx_attrib = 2;

  Vertex vertices[] = {
      {.pos = vec3(-1.0f,  1.0f, 0.0f ), .color = vec3( 1.0f, 0.0f, 0.0f ), .tx = vec2( 0.0, 0.0 ) },
      {.pos = vec3( 1.0f,  1.0f, 0.0f ), .color = vec3( 0.0f, 1.0f, 0.0f ), .tx = vec2( 1.0, 0.0 ) },
      {.pos = vec3( 1.0f, -1.0f, 0.0f ), .color = vec3( 0.0f, 0.0f, 1.0f ), .tx = vec2( 1.0, 1.0 ) },
      {.pos = vec3(-1.0f, -1.0f, 0.0f ), .color = vec3( 1.0f, 1.0f, 1.0f ), .tx = vec2( 0.0, 1.0 ) }};

  Triangle indices[] = {
      {.A = 0, .B = 1, .C = 2},
      {.A = 0, .B = 2, .C = 3}};

  // Create VBO and submit data to GPU buffer
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Create VAO
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Create EBO/Index-Buffer and bind to current VAO
  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Store attribute pointers
  // Vertex Position
  glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, pos));
  glEnableVertexAttribArray(pos_attrib);

  // Vertex Color
  glVertexAttribPointer(color_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));
  glEnableVertexAttribArray(color_attrib);

  // Texture Coordinates
  glVertexAttribPointer(tx_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tx));
  glEnableVertexAttribArray(tx_attrib);

  // Unbind VAO and buffers
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  return VAO;
}


void render_loop( GLFWwindow* window, int initial_width, int initial_height ) {
  glViewport( 0, 0, initial_width, initial_height );
  glClearColor( 0, 0, 0, 1 );

  puts("======== SHADERS ========");
  Shader* shader = new Shader( "shaders/vs.glsl", "shaders/fs.glsl");

  if( !shader->success ) {
    delete shader;
    return;
  }

  GLuint vao = build_vao();

  puts("======== TEXTURES ========");
  Texture tx0 = load_texture( "textures/uv_checker.png" );

  GLuint uniform_tex0 = glGetUniformLocation( shader->m_shader_program, "tex0" );

  GLuint u_mat_model      = glGetUniformLocation( shader->m_shader_program, "model"      );
  GLuint u_mat_view       = glGetUniformLocation( shader->m_shader_program, "view"       );
  GLuint u_mat_projection = glGetUniformLocation( shader->m_shader_program, "projection" );

  glm::mat4 model;     
  glm::mat4 view;      
  glm::mat4 projection;

  model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
  model = glm::rotate(model, glm::radians(-60.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  view       = glm::mat4(1.0f);
  projection = glm::perspective( glm::radians(45.0f), static_cast<float>(1.0f), 0.1f, 100.0f );

  while ( !glfwWindowShouldClose( window ) )
  {
    process_inputs( window );
    glClear( GL_COLOR_BUFFER_BIT );
    shader->use();

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, tx0.obj );
    glUniform1i( uniform_tex0, 0 );

    model = glm::rotate( model, glm::radians( 0.1f ), vec3( 0.0f, 0.0f, 1.0f ) );

    glUniformMatrix4fv( u_mat_model     , 1, GL_FALSE, glm::value_ptr( model      ) );
    glUniformMatrix4fv( u_mat_view      , 1, GL_FALSE, glm::value_ptr( view       ) );
    glUniformMatrix4fv( u_mat_projection, 1, GL_FALSE, glm::value_ptr( projection ) );
    
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers( window );
    glfwPollEvents();
  }

  glDeleteVertexArrays( 1, &vao );
  free_texture( &tx0 );
  delete shader;
}


int main() {
  GLFWwindow* window = init_engine( WIDTH, HEIGHT, TITLE );
  render_loop( window, WIDTH, HEIGHT );
  close_engine( window );
  return 0;
}