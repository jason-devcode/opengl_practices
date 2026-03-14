#define LOG_USE_COLOR
#define STB_IMAGE_IMPLEMENTATION

#include "libs/engine.h"
#include "libs/shader.h"
#include "libs/stb_image.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdint.h>
#include <string.h>

#define WIDTH 512
#define HEIGHT 512
#define TITLE "11 - Texture Units"


typedef struct
{
  union
  {
    struct { float x, y, z; };
    struct { float r, g, b; };
    float a[3];
  };
} vec3;

typedef struct
{
  union
  {
    struct { float u, v; };
    struct { float s, t; };
    float a[2];
  };
} vec2;

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
      {.pos = {-1.0f, 1.0f, 0.0f},  .color = {1.0f, 0.0f, 0.0f}, .tx = { 0.0, 0.0 } },
      {.pos = {1.0f, 1.0f, 0.0f},   .color = {0.0f, 1.0f, 0.0f}, .tx = { 1.0, 0.0 } },
      {.pos = {1.0f, -1.0f, 0.0f},  .color = {0.0f, 0.0f, 1.0f}, .tx = { 1.0, 1.0 } },
      {.pos = {-1.0f, -1.0f, 0.0f}, .color = {1.0f, 1.0f, 1.0f}, .tx = { 0.0, 1.0 } }};

  Triangle indices[] = {
      {.A = 0, .B = 1, .C = 3},
      {.A = 3, .B = 1, .C = 2}};

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

typedef struct {
  int width, height, nrChannels;
  uint8_t* data;
  GLuint obj;
  GLboolean success;
} Texture;

Texture load_texture( const char* filepath ) {
  LOG_INFO( "Loading texture: %s...", filepath );

  int width, height, nrChannels;

  uint8_t* data = stbi_load( filepath, &width, &height, &nrChannels, STBI_rgb);

  LOG_INFO("Texture: %s → width=%d height=%d channels=%d", 
       filepath, width, height, nrChannels);

  if( !data ) {
    LOG_ERROR( "Failed to load %s texture!", filepath );
    return { 0, 0, 0, NULL, 0, GL_FALSE };
  }

  GLuint tx_obj;

  glGenTextures( 1, &tx_obj );
  glBindTexture( GL_TEXTURE_2D, tx_obj );

  // Set texture wrapping
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

  // Set texture filter
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  // Set texture data
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
  
  // Generate mipmaps
  glGenerateMipmap( GL_TEXTURE_2D );

  LOG_SUCCESS( "Texture %s loaded successfully!\n", filepath );

  return (Texture) {
    .width = width, .height = height, .nrChannels = nrChannels,
    .data = data,
    .obj = tx_obj,
    .success = GL_TRUE
  };
}

void free_texture( Texture *texture ) {
  stbi_image_free( texture->data );
  memset( texture, 0, sizeof(Texture) );
}

void test_glm() {
  glm::vec4 vec( 1.0f, 0.0f, 0.0f, 1.0f );
  glm::mat4 trans = glm::mat4(1.0f);
  trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
  vec = trans * vec;
  std::cout << vec.x << ", " << vec.y << ", " << vec.z << "\n";
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
  Texture tx0 = load_texture( "textures/paving_stones.jpg" );
  Texture tx1 = load_texture( "textures/displacement.jpg" );

  GLuint uniform_tex0 = glGetUniformLocation( shader->m_shader_program, "tex0" );
  GLuint uniform_tex1 = glGetUniformLocation( shader->m_shader_program, "tex1" );

  test_glm();

  while ( !glfwWindowShouldClose( window ) )
  {
    process_inputs( window );
    glClear( GL_COLOR_BUFFER_BIT );
    shader->use();

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, tx0.obj );
    glUniform1i( uniform_tex0, 0 );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, tx1.obj );
    glUniform1i( uniform_tex1, 1 );
    
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers( window );
    glfwPollEvents();
  }

  glDeleteVertexArrays( 1, &vao );
  free_texture( &tx0 );
  free_texture( &tx1 );
  delete shader;
}


int main() {
  GLFWwindow* window = init_engine( WIDTH, HEIGHT, TITLE );
  render_loop( window, WIDTH, HEIGHT );
  close_engine( window );
  return 0;
}