#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION

#include "libs/shader.h"
#include "libs/stb_image.h"

int width = 512;
int height = 512;
const char *title = "10 - Textures";

void process_inputs(GLFWwindow *);

typedef struct
{
  union
  {
    struct
    {
      float x, y, z;
    };
    struct
    {
      float r, g, b;
    };
    float a[3];
  };
} vec3;

typedef struct
{
  union
  {
    struct
    {
      float u, v;
    };
    struct
    {
      float s, t;
    };
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

#include <cmath>

void render_loop(GLFWwindow *window)
{
  glViewport(0, 0, width, height);
  glClearColor(0, 0, 0, 1);

  Shader *shader = new Shader("shaders/vs.glsl", "shaders/fs.glsl");

  if (!shader->success){
    delete shader;
    return;
  }

  GLuint vao = build_vao();

  GLuint brightnessLocation = shader->getUniformLocation("brightness");

  float brg = 0.0;

  // Load texture data
  int tx_w, tx_h, nrChannels;
  unsigned char* data = stbi_load( "textures/paving_stones.jpg", &tx_w, &tx_h, &nrChannels, 0 );

  if( not data ) return;

  GLuint texture;
  glGenTextures( 1, &texture );

  glBindTexture( GL_TEXTURE_2D, texture );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, tx_w, tx_h, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
  glGenerateMipmap( GL_TEXTURE_2D );

  while (not glfwWindowShouldClose(window))
  {
    process_inputs(window);
    glClear(GL_COLOR_BUFFER_BIT);

    shader->use();
    glUniform1f(brightnessLocation, static_cast<float>(std::sin(brg) / 2 + 0.5));

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    glfwPollEvents();

    brg += 0.01;
  }
  delete shader;
  stbi_image_free( data );
  glDeleteTextures( 1, &texture );
}

void process_inputs(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  // Press 1 for change to wireframe draw mode
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // Press 2 for change to fill draw mode
  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void framebuffer_size_callback(GLFWwindow *, int w, int h)
{
  glViewport(0, 0, w, h);
}

GLFWwindow *init_engine()
{
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);

  if (window == NULL)
  {
    std::cerr << "Failed to create window\n";
    glfwTerminate();
    return NULL;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (not gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cerr << "Failed to load GL loader \n";
    glfwDestroyWindow(window);
    glfwTerminate();
    return NULL;
  }

  return window;
}

void close_engine(GLFWwindow *window)
{
  if (window)
    glfwDestroyWindow(window);
  glfwTerminate();
}

int main()
{
  GLFWwindow *window = init_engine();
  if (not window)
    return -1;
  render_loop(window);
  close_engine(window);
  return 0;
}
