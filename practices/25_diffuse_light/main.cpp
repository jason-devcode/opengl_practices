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
  // glEnable( GL_CULL_FACE );

  puts("======== SHADERS ========");
  Shader* common_shader = new Shader( "shaders/vs.glsl", "shaders/fs.glsl");
  if( !common_shader->success ) { delete common_shader; return; }

  Shader* light_source_shader = new Shader( "shaders/light_source/vs.glsl", "shaders/light_source/fs.glsl");
  if( !light_source_shader->success ) { delete light_source_shader; return; }
  
  // Mesh cubeMesh = build_cube_mesh();
  Mesh sphereMesh = build_sphere_mesh(
    16, 16, 3.0f, 
    vec3( 1.0f, 1.0f, 1.0f ), 
    vec3( 1.0f, 1.0f, 1.0f ), 
    vec3( 1.0f, 1.0f, 1.0f )
  );

  puts("======== TEXTURES ========");
  Texture tx0 = load_texture( "textures/uv_checker.png" );


  glm::mat4 projection = glm::perspective( glm::radians(45.0f), static_cast<float>(1.0f), 0.1f, 2000.0f );

  float delta_time = 0.0f;
  float last_time = glfwGetTime();
  
  vec3 light_color   = vec3( 1.0f, 1.0f, 1.0f );
  vec3 ambient_light = vec3( 0.1f, 0.1f, 0.1f  );

  glm::mat4 light_model;
  light_model = glm::translate( mat4(1.0f), vec3( 5.0f, 0.0f, 0.0f ) );
  light_model = glm::scale( light_model, vec3( 0.08f ));

  cam.set_position( vec3( 0.0f, 0.0f, 12.0f ) );

  while ( !glfwWindowShouldClose( window ) )
  {
    delta_time = glfwGetTime() - last_time;
    last_time = glfwGetTime();
    process_inputs( window );
    process_camera_inputs( window, delta_time );
    cam.update();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glBindVertexArray( sphereMesh.vao );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, tx0.obj );

    glm::mat4 view = cam.get_lookup_view();
    glm::mat4 model = glm::translate(glm::mat4(1.0f), vec3(0.0f));

    common_shader->use();
    
    light_model = glm::rotate( mat4(1.0f), glm::radians( -180.0f ) * delta_time , vec3( 0.0f, 1.0f, 0.0f ) ) * light_model;
    vec3 light_pos = light_model * vec4( 1.0f, 0.0f, 0.0f, 1.0f );

    common_shader->setVec3( "lightColor"  , light_color   );
    common_shader->setVec3( "lightPos"    , light_pos     );
    common_shader->setVec3( "ambientLight", ambient_light );

    common_shader->setMat4( "model"     , model       );
    common_shader->setMat4( "view"      , view        );
    common_shader->setMat4( "projection", projection  );

    common_shader->setTexUnit( "tex0", 0 );

    glDrawElements(GL_TRIANGLES, sphereMesh.numT * INDEX_PER_TRIANGLE, GL_UNSIGNED_INT, 0);

    light_source_shader->use();
    light_source_shader->setMat4( "model"     , light_model );
    light_source_shader->setMat4( "view"      , view        );
    light_source_shader->setMat4( "projection", projection  );
    light_source_shader->setVec3( "lightColor", light_color );

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
