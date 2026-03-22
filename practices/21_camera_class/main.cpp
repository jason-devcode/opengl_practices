#define LOG_USE_COLOR

#include "libs/engine.h"
#include "libs/shader.h"
#include "libs/texture.h"
#include "libs/camera.h"
#include "libs/mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdint.h>
#include <string.h>

#define WIDTH 512 * 2
#define HEIGHT 512 * 2
#define TITLE "20 - camera lookup from scratch"

using vec3 = glm::vec3;
using vec2 = glm::vec2;

void process_camera_inputs( GLFWwindow* window, Camera& cam, float delta_time );


void render_loop( GLFWwindow* window, int initial_width, int initial_height ) {
  glViewport( 0, 0, initial_width, initial_height );
  glClearColor( 0, 0, 0, 1 );
  glEnable( GL_DEPTH_TEST );

  puts("======== SHADERS ========");
  Shader* shader = new Shader( "shaders/vs.glsl", "shaders/fs.glsl");
  if( !shader->success ) { delete shader; return; }

  Mesh cubeMesh = build_mesh();

  puts("======== TEXTURES ========");
  Texture tx0 = load_texture( "textures/uv_checker.png" );

  int gridW = 70;
  int gridH = 3;
  int gridZ = 70;
  int max_cubes = gridW * gridH * gridZ;
  vec3 cubes[ max_cubes ];
  vec3 rotations[ max_cubes ];

  int cube_index = 0;
  float sparse_factor = 10.0;

  for( int Z = 0; Z < gridZ; Z++ )
  {
    for( int Y = 0; Y < gridH; Y++ )
    {
      for( int X = 0; X < gridW; X++ ) {
        cubes[ cube_index ] = vec3(
          sparse_factor * static_cast<float>(X - gridW / 2),
          sparse_factor * static_cast<float>(Y - gridH / 2),
          sparse_factor * static_cast<float>(Z - gridZ / 2)
        );

        rotations[ cube_index ] = vec3( 
          static_cast<float>( rand() % 100) / 100.0f, 
          static_cast<float>( rand() % 100) / 100.0f, 
          static_cast<float>( rand() % 100) / 100.0f
        );

        ++cube_index;
      }
    }
  }

  // Get shader uniforms
  GLuint uniform_tex0     = glGetUniformLocation( shader->m_shader_program, "tex0"       );

  GLuint u_mat_model      = glGetUniformLocation( shader->m_shader_program, "model"      );
  GLuint u_mat_view       = glGetUniformLocation( shader->m_shader_program, "view"       );
  GLuint u_mat_projection = glGetUniformLocation( shader->m_shader_program, "projection" );

  Camera cam;
  glm::mat4 view;      
  glm::mat4 projection;

  projection = glm::perspective( glm::radians(45.0f), static_cast<float>(1.0f), 0.1f, 2000.0f );

  float delta_time = 0.0f;
  float last_time = glfwGetTime();

  while ( !glfwWindowShouldClose( window ) )
  {
    delta_time = glfwGetTime() - last_time;
    last_time = glfwGetTime();
    process_inputs( window );
    process_camera_inputs( window, cam, delta_time );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    shader->use();

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, tx0.obj );
    glUniform1i( uniform_tex0, 0 );

    cam.update();
    cam.print_info();

    view = cam.get_lookup_view();

    for( int i = 0; i < max_cubes; i++ ) {
      glm::mat4 model = glm::translate(glm::mat4(1.0f), cubes[i]);

      glUniformMatrix4fv( u_mat_model     , 1, GL_FALSE, glm::value_ptr( model      ) );
      glUniformMatrix4fv( u_mat_view      , 1, GL_FALSE, glm::value_ptr( view       ) );
      glUniformMatrix4fv( u_mat_projection, 1, GL_FALSE, glm::value_ptr( projection ) );
      
      glBindVertexArray( cubeMesh.vao );
      glDrawElements(GL_TRIANGLES, cubeMesh.numT * INDEX_PER_TRIANGLE, GL_UNSIGNED_INT, 0);
    }

    glfwSwapBuffers( window );
    glfwPollEvents();
  }

  free_mesh( &cubeMesh );
  free_texture( &tx0 );
  delete shader;
}


int main() {
  GLFWwindow* window = init_engine( WIDTH, HEIGHT, TITLE );
  render_loop( window, WIDTH, HEIGHT );
  close_engine( window );
  return 0;
}


void process_camera_inputs( GLFWwindow* window, Camera& cam, float delta_time ) {
  float cameraSpeed = 100.0f;
  float cameraAngularSpeed = 2.0f;

  if( glfwGetKey( window, GLFW_KEY_W ) )
    cam.move_forward( delta_time * cameraSpeed );

  if( glfwGetKey( window, GLFW_KEY_S ) )
    cam.move_forward( -delta_time * cameraSpeed );

  if( glfwGetKey( window, GLFW_KEY_D ) )
    cam.rotate( vec3( 0.0f, 1.0f, 0.0f ) * -delta_time * cameraAngularSpeed);
  
  if( glfwGetKey( window, GLFW_KEY_A ) ) 
    cam.rotate( vec3( 0.0f, 1.0f, 0.0f ) * delta_time * cameraAngularSpeed);

  if( glfwGetKey( window, GLFW_KEY_R ) ) 
    cam.rotate( vec3( 1.0f, 0.0f, 0.0f ) * delta_time * cameraAngularSpeed);
  
  if( glfwGetKey( window, GLFW_KEY_F ) ) 
    cam.rotate( vec3( 1.0f, 0.0f, 0.0f ) * -delta_time * cameraAngularSpeed);

  if( glfwGetKey( window, GLFW_KEY_Q ) ) 
    cam.rotate( vec3( 0.0f, 0.0f, 1.0f ) * delta_time * cameraAngularSpeed);
  
  if( glfwGetKey( window, GLFW_KEY_E ) ) 
    cam.rotate( vec3( 0.0f, 0.0f, 1.0f ) * -delta_time * cameraAngularSpeed);
}
