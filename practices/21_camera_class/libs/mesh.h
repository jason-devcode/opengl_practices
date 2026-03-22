#ifndef MESH_UTILS_H
#define MESH_UTILS_H

#include <glad/glad.h>

#include <glm/glm.hpp>

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

#endif