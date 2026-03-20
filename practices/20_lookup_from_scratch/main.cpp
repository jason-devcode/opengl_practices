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
#define TITLE "20 - camera lookup from scratch"

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

typedef struct {
  Vertex *vertices;
  Triangle *triangles;
  size_t numV;
  size_t numT;
  GLuint vao;
} Mesh;

void free_mesh( Mesh* mesh ) {
  glDeleteVertexArrays( 1, &mesh->vao );
  mesh->numT = 0;
  mesh->numV = 0;
  delete[] mesh->vertices;
  delete[] mesh->triangles;
}

#include <cstring>

Mesh build_mesh()
{
  Mesh out;

  const unsigned int pos_attrib = 0;
  const unsigned int color_attrib = 1;
  const unsigned int tx_attrib = 2;

  Vertex vertices[] = {
    // Front (+Z)
    { {-1, 1, 1}, {1,0,0}, {0,0} },  // 0
    { { 1, 1, 1}, {0,1,0}, {1,0} },  // 1
    { { 1,-1, 1}, {0,0,1}, {1,1} },  // 2
    { {-1,-1, 1}, {1,1,1}, {0,1} },  // 3

    // Back (-Z)
    { {-1, 1,-1}, {1,0,0}, {0,0} },  // 4
    { { 1, 1,-1}, {0,1,0}, {1,0} },  // 5
    { { 1,-1,-1}, {0,0,1}, {1,1} },  // 6
    { {-1,-1,-1}, {1,1,1}, {0,1} },  // 7

    // Left (-X) 
    { {-1, 1, 1}, {1,0,0}, {0,0} },  // 8
    { {-1,-1, 1}, {1,1,1}, {0,1} },  // 9
    { {-1,-1,-1}, {1,1,1}, {1,1} },  // 10
    { {-1, 1,-1}, {1,0,0}, {1,0} },  // 11

    // Right (+X)
    { { 1, 1, 1}, {0,1,0}, {1,0} },  // 12
    { { 1, 1,-1}, {0,1,0}, {0,0} },  // 13  
    { { 1,-1,-1}, {0,0,1}, {0,1} },  // 14
    { { 1,-1, 1}, {0,0,1}, {1,1} },  // 15

    // Top (+Y)
    { {-1, 1,-1}, {1,0,0}, {0,1} },  // 16
    { { 1, 1,-1}, {0,1,0}, {1,1} },  // 17
    { { 1, 1, 1}, {0,1,0}, {1,0} },  // 18
    { {-1, 1, 1}, {1,0,0}, {0,0} },  // 19

    // Bottom (-Y)
    { {-1,-1,-1}, {1,1,1}, {0,1} },  // 20
    { {-1,-1, 1}, {1,1,1}, {0,0} },  // 21
    { { 1,-1, 1}, {0,0,1}, {1,0} },  // 22
    { { 1,-1,-1}, {0,0,1}, {1,1} },  // 23
  };

  Triangle triangles[] = {
    // Front
    {0,1,2}, {0,2,3},
    // Back
    {4,5,6}, {4,6,7},
    // Left
    {8,9,10}, {8,10,11},
    // Right
    {12,13,14}, {12,14,15},
    // Top
    {16,17,18}, {16,18,19},
    // Bottom
    {20,21,22}, {20,22,23},
  };

  // Set Mesh Data
  int numVertices = sizeof( vertices ) / sizeof( Vertex );
  int numTriangles = sizeof( triangles ) / sizeof( Triangle );

  out.vertices = new Vertex[ numVertices ];
  std::memcpy( out.vertices, vertices, sizeof(vertices) );
  
  out.triangles = new Triangle[ numTriangles ];
  std::memcpy( out.triangles, triangles, sizeof(triangles) );

  out.numV = numVertices;
  out.numT = numTriangles;

  // Create VBO and submit data to GPU buffer
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), out.vertices, GL_STATIC_DRAW);

  // Create VAO
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Create EBO/Index-Buffer and bind to current VAO
  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles), out.triangles, GL_STATIC_DRAW);

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

  out.vao = VAO;

  return out;
}


void render_loop( GLFWwindow* window, int initial_width, int initial_height ) {
  glViewport( 0, 0, initial_width, initial_height );
  glClearColor( 0, 0, 0, 1 );
  glEnable( GL_DEPTH_TEST );

  puts("======== SHADERS ========");
  Shader* shader = new Shader( "shaders/vs.glsl", "shaders/fs.glsl");

  if( !shader->success ) {
    delete shader;
    return;
  }

  Mesh cubeMesh = build_mesh();

  puts("======== TEXTURES ========");
  Texture tx0 = load_texture( "textures/uv_checker.png" );

  GLuint uniform_tex0 = glGetUniformLocation( shader->m_shader_program, "tex0" );

  GLuint u_mat_model      = glGetUniformLocation( shader->m_shader_program, "model"      );
  GLuint u_mat_view       = glGetUniformLocation( shader->m_shader_program, "view"       );
  GLuint u_mat_projection = glGetUniformLocation( shader->m_shader_program, "projection" );

  // glm::mat4 model;     
  glm::mat4 view;      
  glm::mat4 projection;

  // model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
  // model = glm::rotate(model, glm::radians(-60.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -6.0f));

  


  projection = glm::perspective( glm::radians(45.0f), static_cast<float>(1.0f), 0.1f, 2000.0f );

  int gridW = 35;
  int gridH = 1;
  int gridZ = 35;
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

  vec3 cameraPos   = vec3( 0.0f, 0.0f, 10.0f );
  vec3 cameraFront = vec3( 0.0f, 0.0f, -1.0f );

  float cameraSpeed = 0.5;
  float cameraAngle = glm::pi<float>();

  while ( !glfwWindowShouldClose( window ) )
  {
    process_inputs( window );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    shader->use();

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, tx0.obj );
    glUniform1i( uniform_tex0, 0 );

    if( glfwGetKey( window, GLFW_KEY_W ) ) cameraPos -= cameraSpeed * cameraFront;
    if( glfwGetKey( window, GLFW_KEY_S ) ) cameraPos += cameraSpeed * cameraFront;

    if( glfwGetKey( window, GLFW_KEY_A ) ){ 
      cameraAngle -= 0.01;
      cameraFront.x = sin( cameraAngle );
      cameraFront.z = cos( cameraAngle );
    }
    if( glfwGetKey( window, GLFW_KEY_D ) ){ 
      cameraAngle += 0.01;
      cameraFront.x = sin( cameraAngle );
      cameraFront.z = cos( cameraAngle );
    }

    vec3 cameraRight = glm::cross( cameraFront, vec3( 0.0, 1.0, 0.0 ));

    // Calc correct camera up
    vec3 cameraUp = glm::cross( cameraRight, cameraFront );

    std::cout << "camera up: " << cameraUp.x << ", " << cameraUp.y << ", " << cameraUp.z << "\n";

    view = glm::mat4( 
      glm::vec4(cameraRight, 0.0f), 
      glm::vec4(cameraUp, 0.0f), 
      glm::vec4(cameraFront, 0.0f), 
      glm::vec4( 0.0f ,0.0f ,0.0f, 1.0f ) 
    );
    view = glm::translate( view, -cameraPos );
    
    for( int i = 0; i < max_cubes; i++ ) {
      glm::mat4 model = glm::translate(glm::mat4(1.0f), cubes[i]);
      // model = glm::rotate( model, static_cast<float>( glm::radians( 45.0f ) * glfwGetTime() ), rotations[ i ] );

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