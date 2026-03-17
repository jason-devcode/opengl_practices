#define LOG_USE_COLOR

#include "libs/engine.h"
#include "libs/shader.h"
#include "libs/texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdint.h>
#include <string.h>

#define WIDTH 900
#define HEIGHT 900
#define TITLE "15 - Pseudo 3D Effect + Sprite Rendering"

using vec3 = glm::vec3;
using vec2 = glm::vec2;

typedef struct
{
  vec3 pos;
  vec3 color;
  vec2 index;
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
      {.pos = vec3(-1.0f,  1.0f, 0.0f ), .color = vec3( 1.0f, 0.0f, 0.0f ), .index = { 3.0f, 0.0f }},
      {.pos = vec3( 1.0f,  1.0f, 0.0f ), .color = vec3( 0.0f, 1.0f, 0.0f ), .index = { 4.0f, 0.0f }},
      {.pos = vec3( 1.0f, -1.0f, 0.0f ), .color = vec3( 0.0f, 0.0f, 1.0f ), .index = { 2.0f, 0.0f }},
      {.pos = vec3(-1.0f, -1.0f, 0.0f ), .color = vec3( 1.0f, 1.0f, 1.0f ), .index = { 1.0f, 0.0f }}};

  Triangle indices[] = {
    {.A = 1, .B = 2, .C = 3},
    {.A = 3, .B = 1, .C = 0}
    };

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

  // Index value
  glVertexAttribPointer(tx_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, index));
  glEnableVertexAttribArray(tx_attrib);

  // Unbind VAO and buffers
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  return VAO;
}


GLuint build_sprite_vao()
{
  const unsigned int pos_attrib = 0;
  const unsigned int color_attrib = 1;
  const unsigned int tx_attrib = 2;

  Vertex vertices[] = {
      {.pos = vec3(-1.0f,  1.0f, 0.0f ), .color = vec3( 1.0f, 0.0f, 0.0f ), { 0.0f, 1.0f }},
      {.pos = vec3( 1.0f,  1.0f, 0.0f ), .color = vec3( 0.0f, 1.0f, 0.0f ), { 1.0f, 1.0f }},
      {.pos = vec3( 1.0f, -1.0f, 0.0f ), .color = vec3( 0.0f, 0.0f, 1.0f ), { 1.0f, 0.0f }},
      {.pos = vec3(-1.0f, -1.0f, 0.0f ), .color = vec3( 1.0f, 1.0f, 1.0f ), { 0.0f, 0.0f }}};

  Triangle indices[] = {
    {.A = 1, .B = 2, .C = 3},
    {.A = 3, .B = 1, .C = 0}
    };

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

  // Texture value
  glVertexAttribPointer(tx_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, index));
  glEnableVertexAttribArray(tx_attrib);

  // Unbind VAO and buffers
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  return VAO;
}


void render_loop( GLFWwindow* window, int initial_width, int initial_height ) {
  glViewport( 0, 0, initial_width, initial_height );
  glClearColor( 0, 0.5, 1.0, 1 );

  puts("======== SHADERS ========");
  Shader* shader = new Shader( "shaders/vs.glsl", "shaders/fs.glsl");
  Shader* sprite_shader = new Shader( "shaders/sprite_vs.glsl", "shaders/sprite_fs.glsl");

  if( !shader->success || !sprite_shader->success ) {
    delete shader;
    delete sprite_shader;
    return;
  }

  GLuint vao = build_vao();
  GLuint sprite_vao = build_sprite_vao();


  stbi_set_flip_vertically_on_load(1);

  puts("======== TEXTURES ========");
  Texture tx0 = load_texture( "textures/mario_circuit_1.png" );
  Texture tx1 = load_texture( "textures/sky.jpg" );
  
  Texture kart_tx = load_texture( "textures/kart.png", true );

  GLuint u_sprite = glGetUniformLocation( sprite_shader->m_shader_program, "sprite");
  GLuint u_yOffset = glGetUniformLocation( sprite_shader->m_shader_program, "yOffset");

  GLuint uniform_tex0 = glGetUniformLocation( shader->m_shader_program, "tex0" );
  GLuint uniform_tex1 = glGetUniformLocation( shader->m_shader_program, "tex1" );

  GLuint uniform_transform = glGetUniformLocation( shader->m_shader_program, "transform" );

  glm::mat4 m = glm::mat4(1.0f);

  float scale = 1.0f;

  // Get uv distances uniforms
  GLuint u_uvFar1 = glGetUniformLocation( shader->m_shader_program, "uvFar1" );
  GLuint u_uvFar2 = glGetUniformLocation( shader->m_shader_program, "uvFar2" );
  GLuint u_uvNear1 = glGetUniformLocation( shader->m_shader_program, "uvNear1" );
  GLuint u_uvNear2 = glGetUniformLocation( shader->m_shader_program, "uvNear2" );

  vec2 uvFar1 =  vec2( 0.0f, 0.0f );
  vec2 uvFar2 =  vec2( 1.0f, 0.0f );
  vec2 uvNear1 = vec2( 1.0f, 1.0f );
  vec2 uvNear2 = vec2( 0.0f, 1.0f );

  vec2 camera_pos = vec2(0.0f);

  float camera_angle = 0.0f;
  float far = 5.0;
  float near = 0.02;
  float fov = 70;

  float yOffset = 0.0;
  float gravity = 0.1;
  float velY = 0.0;

  GLuint u_uNear = glGetUniformLocation(shader->m_shader_program, "uNear");
  GLuint u_uFar  = glGetUniformLocation(shader->m_shader_program, "uFar");
  GLuint u_Sky  = glGetUniformLocation(shader->m_shader_program, "uSky");



  while ( !glfwWindowShouldClose( window ) )
  {
    process_inputs( window );
    glClear( GL_COLOR_BUFFER_BIT );
    shader->use();

    glBindVertexArray(vao);

    glUniform1i( u_Sky, 0 );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, tx0.obj );
    glUniform1i( uniform_tex0, 0 );

    glUniformMatrix4fv( uniform_transform, 1, GL_FALSE, glm::value_ptr(m) );

    if( glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS && yOffset == 0.0 ) velY = 0.02;
    yOffset += velY;
    
    if( yOffset > 0.0 ) { 
      velY -= gravity * 0.008;
    }

    if( yOffset <= 0.0 ) {
      velY = 0.0;
      yOffset = 0.0;
    }
    

    if( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS ) camera_pos += vec2( cos( glm::radians( camera_angle ) ), sin( glm::radians( camera_angle ) )) * vec2(0.001);
    if( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS ) camera_pos -= vec2( cos( glm::radians( camera_angle ) ), sin( glm::radians( camera_angle ) )) * vec2(0.001);
    if( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS ) camera_angle += 1;
    if( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS ) camera_angle -= 1;

    if( glfwGetKey( window, GLFW_KEY_Q ) == GLFW_PRESS ) fov -= 0.1;
    if( glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS ) fov += 0.1;

    if( glfwGetKey( window, GLFW_KEY_R ) == GLFW_PRESS ) far += 0.1;
    if( glfwGetKey( window, GLFW_KEY_F ) == GLFW_PRESS ) far -= 0.1;

    if( glfwGetKey( window, GLFW_KEY_T ) == GLFW_PRESS ) near += 0.001;
    if( glfwGetKey( window, GLFW_KEY_G ) == GLFW_PRESS ) near -= 0.001;
    

    uvFar1 = camera_pos + far * vec2( cos( glm::radians( camera_angle - fov * 0.5 ) ), sin( glm::radians( camera_angle - fov * 0.5 ) ));
    uvFar2 = camera_pos + far * vec2( cos( glm::radians( camera_angle + fov * 0.5 ) ), sin( glm::radians( camera_angle + fov * 0.5 ) ));

    uvNear1 = camera_pos +  near * vec2( cos( glm::radians( camera_angle - fov * 0.5 ) ), sin( glm::radians( camera_angle - fov * 0.5 ) ));
    uvNear2 = camera_pos +  near * vec2( cos( glm::radians( camera_angle + fov * 0.5 ) ), sin( glm::radians( camera_angle + fov * 0.5 ) ));
    
    glUniform2fv( u_uvFar1, 1, glm::value_ptr( uvFar1 ) );
    glUniform2fv( u_uvFar2, 1, glm::value_ptr( uvFar2 ) );
    glUniform2fv( u_uvNear1, 1, glm::value_ptr( uvNear1 ) );
    glUniform2fv( u_uvNear2, 1, glm::value_ptr( uvNear2 ) );

    glUniform1f(u_uNear, near);
    glUniform1f(u_uFar,  far);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUniform1i( u_Sky, 1 );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, tx1.obj );
    glUniform1i( uniform_tex0, 0 );

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Draw kart
    sprite_shader->use();
    glBindVertexArray( sprite_vao );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, kart_tx.obj );
    glUniform1i( u_sprite, 0 );
    glUniform1f( u_yOffset, yOffset );

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDisable(GL_BLEND);
    
    glfwSwapBuffers( window );
    glfwPollEvents();
  }

  glDeleteVertexArrays( 1, &vao );
  free_texture( &tx0 );
  free_texture( &tx1 );
  free_texture( &kart_tx );
  
  delete shader;
  delete sprite_shader;
}


int main() {
  GLFWwindow* window = init_engine( WIDTH, HEIGHT, TITLE );
  render_loop( window, WIDTH, HEIGHT );
  close_engine( window );
  return 0;
}