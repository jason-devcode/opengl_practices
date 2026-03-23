#define LOG_USE_COLOR

#include "libs/engine.h"
#include "libs/shader.h"
#include "libs/texture.h"
#include "libs/camera.h"
#include "libs/camera_inputs.h"
#include "libs/mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdint.h>
#include <string.h>

#define WIDTH 512 * 2
#define HEIGHT 512 * 2
#define TITLE "24 - linear light"

using vec3 = glm::vec3;
using vec2 = glm::vec2;


void render_loop( GLFWwindow* window, int initial_width, int initial_height ) {
  glfwSetCursorPosCallback( window, mouse_callback );

  glViewport( 0, 0, initial_width, initial_height );
  glClearColor( 0, 0, 0, 1 );
  glEnable( GL_DEPTH_TEST );

  puts("======== SHADERS ========");
  Shader* common_shader = new Shader( "shaders/vs.glsl", "shaders/fs.glsl");
  if( !common_shader->success ) { delete common_shader; return; }

  Shader* light_source_shader = new Shader( "shaders/light_source/vs.glsl", "shaders/light_source/fs.glsl");
  if( !light_source_shader->success ) { delete light_source_shader; return; }
  

  // Mesh cubeMesh = build_cube_mesh();
  Mesh sphereMesh = build_sphere_mesh(
    64, 64, 12.0f, 
    vec3( 1.0f, 1.0f, 1.0f ), 
    vec3( 1.0f, 1.0f, 1.0f ), 
    vec3( 1.0f, 1.0f, 1.0f )
  );

  puts("======== TEXTURES ========");
  Texture tx0 = load_texture( "textures/uv_checker.png" );

  // Get shader uniforms
  GLuint uniform_tex0 = glGetUniformLocation( common_shader->m_shader_program, "tex0" );

  glm::mat4 view;      
  glm::mat4 projection;

  projection = glm::perspective( glm::radians(45.0f), static_cast<float>(1.0f), 0.1f, 2000.0f );

  float delta_time = 0.0f;
  float last_time = glfwGetTime();

  cam.set_position( vec3( 0.0f, 0.0f, 40.0f ) );
  
  glBindVertexArray( sphereMesh.vao );
  
  vec3 light_pos   = vec3( 12.0f, 12.0f, 5.0f );
  vec3 light_color = vec3( 0.0f, 1.0f,  1.0f );

  glm::mat4 light_model;
  light_model = glm::translate( glm::mat4(1.0f), light_pos );
  light_model = glm::scale( light_model, vec3( 0.08f ));

  while ( !glfwWindowShouldClose( window ) )
  {
    delta_time = glfwGetTime() - last_time;
    last_time = glfwGetTime();
    process_inputs( window );
    process_camera_inputs( window, delta_time );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    common_shader->use();

    GLuint u_mat_model      = glGetUniformLocation( common_shader->m_shader_program, "model"      );
    GLuint u_mat_view       = glGetUniformLocation( common_shader->m_shader_program, "view"       );
    GLuint u_mat_projection = glGetUniformLocation( common_shader->m_shader_program, "projection" );
    GLuint u_light_color    = glGetUniformLocation( common_shader->m_shader_program, "lightColor" );
    GLuint u_light_pos      = glGetUniformLocation( common_shader->m_shader_program, "lightPos" );
    
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, tx0.obj );
    glUniform1i( uniform_tex0, 0 );

    cam.update();

    view = cam.get_lookup_view();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), vec3(0.0f));

    glUniformMatrix4fv( u_mat_model     , 1, GL_FALSE, glm::value_ptr( model      ) );
    glUniformMatrix4fv( u_mat_view      , 1, GL_FALSE, glm::value_ptr( view       ) );
    glUniformMatrix4fv( u_mat_projection, 1, GL_FALSE, glm::value_ptr( projection ) );
    
    glUniform3fv( u_light_color, 1, glm::value_ptr( light_color ) );
    glUniform3fv( u_light_pos  , 1, glm::value_ptr( light_pos   ) );

    glDrawElements(GL_TRIANGLES, sphereMesh.numT * INDEX_PER_TRIANGLE, GL_UNSIGNED_INT, 0);

    light_source_shader->use();

    u_mat_model      = glGetUniformLocation( light_source_shader->m_shader_program, "model"      );
    u_mat_view       = glGetUniformLocation( light_source_shader->m_shader_program, "view"       );
    u_mat_projection = glGetUniformLocation( light_source_shader->m_shader_program, "projection" );

    u_light_color    = glGetUniformLocation( light_source_shader->m_shader_program, "lightColor" );

    glUniformMatrix4fv( u_mat_model     , 1, GL_FALSE, glm::value_ptr( light_model ) );
    glUniformMatrix4fv( u_mat_view      , 1, GL_FALSE, glm::value_ptr( view        ) );
    glUniformMatrix4fv( u_mat_projection, 1, GL_FALSE, glm::value_ptr( projection  ) );
    
    glUniform3fv( u_light_color, 1, glm::value_ptr( light_color ) );

    glDrawElements(GL_TRIANGLES, sphereMesh.numT * INDEX_PER_TRIANGLE, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers( window );
    glfwPollEvents();
  }

  free_mesh( &sphereMesh );
  free_texture( &tx0 );

  delete common_shader;
  delete light_source_shader;
}


int main() {
  GLFWwindow* window = init_engine( WIDTH, HEIGHT, TITLE );
  render_loop( window, WIDTH, HEIGHT );
  close_engine( window );
  return 0;
}
