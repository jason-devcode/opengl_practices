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
  vec3 normal;
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

Mesh build_cube_mesh()
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







// Interpola entre 3 colores según t ∈ [0,1]
// t=0 → c0 (polo top), t=0.5 → c1 (ecuador), t=1 → c2 (polo bottom)
static glm::vec3 tricolor_lerp(glm::vec3 c0, glm::vec3 c1, glm::vec3 c2, float t)
{
    if (t <= 0.5f)
        return glm::mix(c0, c1, t * 2.0f);
    else
        return glm::mix(c1, c2, (t - 0.5f) * 2.0f);
}

Mesh build_sphere_mesh(
    int stacks,           // divisiones verticales (≥ 2, ej: 16)
    int slices,           // divisiones horizontales (≥ 3, ej: 32)
    float radius,      // radio de la esfera
    glm::vec3 color_top,      // color polo norte  (y = +1)
    glm::vec3 color_equator,  // color ecuador     (y =  0)
    glm::vec3 color_bottom    // color polo sur    (y = -1)
)
{
    const unsigned int pos_attrib       = 0;
    const unsigned int color_attrib     = 1;
    const unsigned int tx_attrib        = 2;
    const unsigned int normal_attrib    = 3;

    // ── Generar vértices ──────────────────────────────────────────────────────
    // (stacks+1) anillos × (slices+1) vértices por anillo
    // El +1 en slices duplica el vértice del meridiano 0/360 para que las UVs
    // no tengan salto: u=0 y u=1 son el mismo punto 3D pero distinta UV.
    int numVertices = (stacks + 1) * (slices + 1);
    Vertex* vertices = new Vertex[numVertices];

    int vi = 0;
    for (int i = 0; i <= stacks; ++i)
    {
        // phi: 0 (polo norte) → π (polo sur)
        float phi = M_PI * (float)i / (float)stacks;
        float v   = (float)i / (float)stacks;   // UV.y: 0 arriba, 1 abajo

        for (int j = 0; j <= slices; ++j)
        {
            // theta: 0 → 2π (vuelta completa)
            float theta = 2.0f * M_PI * (float)j / (float)slices;
            float u     = 1.0f - (float)j / (float)slices;  // UV.x

            // Coordenadas normalizadas
            float nx = std::sin(phi) * std::cos(theta);
            float ny = std::cos(phi);
            float nz = std::sin(phi) * std::sin(theta);

            // Posición en la esfera unitaria
            float x = nx * radius;
            float y = ny * radius;
            float z = nz * radius;

            glm::vec3 color = tricolor_lerp(color_top, color_equator, color_bottom, v);

            vertices[vi++] = {
                { x, y, z },
                { color.r, color.g, color.b },
                { u, v },
                { nx, ny, nz }
            };
        }
    }

    // ── Generar triángulos ────────────────────────────────────────────────────
    // Cada quad (i, j) se divide en 2 triángulos.
    // Total de quads: stacks × slices → stacks × slices × 2 triángulos.
    int numTriangles = stacks * slices * 2;
    Triangle* triangles = new Triangle[numTriangles];

    int ti = 0;
    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < slices; ++j)
        {
            // Índices de las 4 esquinas del quad
            unsigned int top_left     = i       * (slices + 1) + j;
            unsigned int top_right    = i       * (slices + 1) + j + 1;
            unsigned int bottom_left  = (i + 1) * (slices + 1) + j;
            unsigned int bottom_right = (i + 1) * (slices + 1) + j + 1;

            // Triángulo superior-izquierdo del quad
            triangles[ti++] = { top_left,  bottom_left,  top_right };
            // Triángulo inferior-derecho del quad
            triangles[ti++] = { top_right, bottom_left,  bottom_right };
        }
    }

    // ── Construir el Mesh (igual que build_cube_mesh) ─────────────────────────
    Mesh out;
    out.numV = numVertices;
    out.numT = numTriangles;

    out.vertices  = new Vertex[numVertices];
    std::memcpy(out.vertices, vertices, numVertices * sizeof(Vertex));

    out.triangles = new Triangle[numTriangles];
    std::memcpy(out.triangles, triangles, numTriangles * sizeof(Triangle));

    delete[] vertices;
    delete[] triangles;

    // ── Upload a GPU ──────────────────────────────────────────────────────────
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), out.vertices, GL_STATIC_DRAW);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numTriangles * sizeof(Triangle), out.triangles, GL_STATIC_DRAW);

    glVertexAttribPointer(pos_attrib,   3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(pos_attrib);

    glVertexAttribPointer(color_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(color_attrib);

    glVertexAttribPointer(tx_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tx));
    glEnableVertexAttribArray(tx_attrib);

    glVertexAttribPointer(normal_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(normal_attrib);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    out.vao = VAO;
    return out;
}

Mesh build_plane_mesh(
    int subdivisions_x,       // subdivisiones en X (≥ 1, ej: 10)
    int subdivisions_z,       // subdivisiones en Z (≥ 1, ej: 10)
    float width,              // ancho total en X
    float depth,              // profundidad total en Z
    glm::vec3 offset,         // offset de posicion
    glm::vec3 color_a,        // color esquina (−X, −Z)
    glm::vec3 color_b         // color esquina (+X, +Z)
)
{
    const unsigned int pos_attrib    = 0;
    const unsigned int color_attrib  = 1;
    const unsigned int tx_attrib     = 2;
    const unsigned int normal_attrib = 3;

    // ── Generar vértices ──────────────────────────────────────────────────────
    // (subdivisions_x+1) columnas × (subdivisions_z+1) filas
    int cols = subdivisions_x + 1;
    int rows = subdivisions_z + 1;
    int numVertices = cols * rows;
    Vertex* vertices = new Vertex[numVertices];

    int vi = 0;
    for (int iz = 0; iz < rows; ++iz)
    {
        float t  = (float)iz / (float)subdivisions_z;   // 0 → 1 en Z
        float v  = t;                                    // UV.y

        for (int ix = 0; ix < cols; ++ix)
        {
            float s  = (float)ix / (float)subdivisions_x;  // 0 → 1 en X
            float u  = s;                                   // UV.x

            float x  = (s - 0.5f) * width;   // centrado en origen
            float y  = 0.0f;                  // plano plano en Y=0
            float z  = (t - 0.5f) * depth;

            // Interpolar color bilineal entre las dos esquinas
            glm::vec3 color = glm::mix(color_a, color_b, (s + t) * 0.5f);

            vertices[vi++] = {
                vec3(x, y, z) + offset,
                { color.r, color.g, color.b },
                { u, v },
                { 0.0f, 1.0f, 0.0f }   // normal apuntando a Y+
            };
        }
    }

    // ── Generar triángulos ────────────────────────────────────────────────────
    // Cada quad (ix, iz) → 2 triángulos. Total: subdivisions_x × subdivisions_z × 2
    int numTriangles = subdivisions_x * subdivisions_z * 2;
    Triangle* triangles = new Triangle[numTriangles];

    int ti = 0;
    for (int iz = 0; iz < subdivisions_z; ++iz)
    {
        for (int ix = 0; ix < subdivisions_x; ++ix)
        {
            unsigned int top_left     =  iz      * cols + ix;
            unsigned int top_right    =  iz      * cols + ix + 1;
            unsigned int bottom_left  = (iz + 1) * cols + ix;
            unsigned int bottom_right = (iz + 1) * cols + ix + 1;

            // Winding CCW visto desde Y+ (norma OpenGL para caras frontales)
            triangles[ti++] = { top_left,  bottom_left,  top_right    };
            triangles[ti++] = { top_right, bottom_left,  bottom_right };
        }
    }

    // ── Construir el Mesh ─────────────────────────────────────────────────────
    Mesh out;
    out.numV = numVertices;
    out.numT = numTriangles;

    out.vertices  = new Vertex[numVertices];
    std::memcpy(out.vertices, vertices, numVertices * sizeof(Vertex));

    out.triangles = new Triangle[numTriangles];
    std::memcpy(out.triangles, triangles, numTriangles * sizeof(Triangle));

    delete[] vertices;
    delete[] triangles;

    // ── Upload a GPU ──────────────────────────────────────────────────────────
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), out.vertices, GL_STATIC_DRAW);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numTriangles * sizeof(Triangle), out.triangles, GL_STATIC_DRAW);

    glVertexAttribPointer(pos_attrib,    3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(pos_attrib);

    glVertexAttribPointer(color_attrib,  3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(color_attrib);

    glVertexAttribPointer(tx_attrib,     2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tx));
    glEnableVertexAttribArray(tx_attrib);

    glVertexAttribPointer(normal_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(normal_attrib);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    out.vao = VAO;
    return out;
}

#endif