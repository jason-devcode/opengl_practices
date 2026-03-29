#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SCREEN_W 1024
#define SCREEN_H 1024

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

std::ostream& operator<<( std::ostream& out, const vec2& input ) {
  out << input.x << ", " << input.y;
  return out;
}

std::ostream& operator<<( std::ostream& out, const vec3& input ) {
  out << input.x << ", " << input.y << ", " << input.z;
  return out;
}

std::ostream& operator<<( std::ostream& out, const vec4& input ) {
  out << input.x << ", " << input.y << ", " << input.z << ", " << input.w;
  return out;
}

typedef struct {
  vec3 pos;
  vec3 color;
  vec3 normal;
  vec2 txcoord;
  vec3 sphrNormal;
} Vertex;

typedef struct {
  int A, B, C;
} Triangle;

#define INDEX_PER_TRIANGLE 3

typedef struct {
  Vertex *vertices;
  Triangle *triangles;
  size_t numVertices;
  size_t numTriangles;
  GLuint vao;
} Mesh;

Mesh gen_cube( int cols, int rows, float spherizeRatio ) {
  vec3 axis[] = {
    vec3( 1.0f, 0.0f,  0.0f ), // right
    vec3( 0.0f, 1.0f,  0.0f ), // up
    vec3( 0.0f, 0.0f,  1.0f ), // front

    vec3( -1.0f, 0.0f,  0.0f ), // left
    vec3( 0.0f, -1.0f,  0.0f ), // bottom
    vec3( 0.0f, 0.0f,  -1.0f ), // back
  };

  int faceAxis[] = {
// right, up, front
    0,    1,  2, // front
    0,    5,  1, // up
    2,    1,  3, // left
    5,    1,  0, // right
    3,    1,  5, // back
    0,    2,  4  // bottom
  };

  size_t numFaces = 6;

  size_t numVertices = ( cols + 1 ) * ( rows + 1 ) * numFaces;

  size_t numTriangles = cols * rows * numFaces * 2; // 2 triangles per face/normal

  Vertex *vertices = new Vertex[ numVertices ];
  Triangle *triangles = new Triangle[ numTriangles ];

  // Generate all vertices
  for( int faceIndex = 0; faceIndex < numFaces; faceIndex++ ) {
    vec3 right = axis[ faceAxis[ (faceIndex * 3 ) + 0 ] ];
    vec3 up    = axis[ faceAxis[ (faceIndex * 3 ) + 1 ] ];
    vec3 front = axis[ faceAxis[ (faceIndex * 3 ) + 2 ] ];

    for( int j = 0; j < (rows + 1); j++ ) {
      for( int i = 0; i < (cols + 1); i++ ) {
        int vertexIndex =  faceIndex * (cols + 1) * (rows + 1) + ( j * (cols + 1) ) + i;

        float rx = (float) i / (float) cols - 0.5f;
        float ry = (float) j / (float) rows - 0.5f;

        vec3 pos = right * rx + up * ry + front * 0.5f;
        vertices[ vertexIndex ] = (Vertex){
          pos * (1.0f - spherizeRatio) + glm::normalize( pos ) * spherizeRatio,
          vec3( 0.0f, 0.0f, 0.2f ), 
          // vec3( (float)i / (float)( cols ), (float) faceIndex / (float) numFaces, (float) j / (float)rows ), 
          front,
          vec2( (float) i / (float) (cols + 1), (float) j / (float) (rows + 1) ), { 0.0f, 0.0f, 0.0f }
        };

      }
    }
  }

  // Generate all triangles
  for( int faceIndex = 0; faceIndex < numFaces; faceIndex++ ) {
    int vertexIndex = faceIndex * (cols + 1) * (rows + 1);

    for( int j = 0; j < rows; j++ ) {
      for( int i = 0; i < cols; i++ ) {
        int quadIndex = faceIndex * cols * rows * 2;
        int triangleIndex = quadIndex + (j * cols * 2) + i * 2;

        int indexA = vertexIndex + (j * (cols + 1)) + i;
        int indexB = vertexIndex + (j * (cols + 1)) + i + 1;
        int indexC = vertexIndex + ((j + 1) * (cols + 1)) + i + 1;
        int indexD = vertexIndex + ((j + 1) * (cols + 1)) + i;

        triangles[ triangleIndex    ] = (Triangle) { indexA, indexB, indexC };
        triangles[ triangleIndex + 1] = (Triangle) { indexA, indexC, indexD };

        Vertex* A = &vertices[ indexA ];
        Vertex* B = &vertices[ indexB ];
        Vertex* C = &vertices[ indexC ];
        Vertex* D = &vertices[ indexD ];

        // Calculate spheric normals
        vec3 n = glm::normalize( glm::cross( glm::normalize( A->pos ) - glm::normalize( C->pos ), glm::normalize( B->pos ) - glm::normalize( C->pos ) ) );

        D->sphrNormal = C->sphrNormal = B->sphrNormal = A->sphrNormal = n;
      }
    }
  }

  // Build vao
  GLuint vao;
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );

  // Build VBO
  GLuint vbo;
  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertices, GL_STATIC_DRAW );

  // Set VBO Attribute pointers
  // pos
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*) offsetof( Vertex, pos ) );
  glEnableVertexAttribArray(0);
  
  // color
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*) offsetof( Vertex, color ) );
  glEnableVertexAttribArray(1);

  // normal
  glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*) offsetof( Vertex, normal ) );
  glEnableVertexAttribArray(2);

  // txcoord
  glVertexAttribPointer( 3, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*) offsetof( Vertex, txcoord ) );
  glEnableVertexAttribArray(3);
  
  // sphrNormal
  glVertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*) offsetof( Vertex, sphrNormal ) );
  glEnableVertexAttribArray(4);


  // Set EBO
  GLuint ebo;
  glGenBuffers( 1, &ebo );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle) * numTriangles, triangles, GL_STATIC_DRAW );

  // Unbind vao
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return (Mesh){
    vertices, triangles, numVertices, numTriangles, vao
  };
}


void free_mesh( Mesh& m ) {
  delete[] m.vertices;
  delete[] m.triangles;
}

const char* vs_src = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;
layout (location = 4) in vec3 aSphrNormal;

out vec3 vColor;
out vec3 vNormal;
out vec3 vPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float spherization;
uniform float scale;

void main() {
  vec3 sphrPos = (aPos * (1.0f - spherization) + normalize(aPos) * spherization) * scale;
  vec4 sphrNorm = (model * vec4( aNormal, 1.0 )) * (1.0f - spherization ) + (model * vec4(aSphrNormal, 1.0)) * spherization;

  vColor  = aColor;
  vNormal = normalize( sphrNorm.xyz );
  vPos    = sphrPos;

  gl_Position = projection * view * model * vec4( sphrPos, 1.0 );
}
)";

const char* fs_src = R"(
#version 330 core

in vec3 vColor;
in vec3 vNormal;
in vec3 vPos;

out vec4 FragColor;

uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 ambient_light;

uniform vec3 eye_pos;

void main() {
  vec3 light_dir = normalize( light_pos - vPos );
  float specularStrength = 0.5;
  
  float diff = max( 0.0, dot( light_dir, vNormal  ));

  vec3 reflect_dir = reflect( -light_dir, vNormal );
  vec3 view_dir    = normalize( eye_pos - vPos );
  float spec       = pow( max( dot( view_dir, reflect_dir ), 0.0 ), 256 );

  vec3 diffuse = light_color * diff;
  vec3 specular = light_color * spec;

  FragColor = vec4( vColor * (ambient_light + diffuse) + specular , 1.0);
}
)";

const char* fs_light_src = R"(
#version 330 core

out vec4 FragColor;

uniform vec3 light_color;

void main() {
  FragColor = vec4(light_color, 1.0);
}
)";


GLuint process_shaders( const char* vertex_shader_src, const char* fragment_shader_src ) {
  GLint success = GL_FALSE;

  GLuint vs = glCreateShader( GL_VERTEX_SHADER );
  glShaderSource( vs, 1, &vertex_shader_src, NULL );
  glCompileShader( vs );
  glGetShaderiv( vs, GL_COMPILE_STATUS, &success );

  if( !success ) {
    char info_log[1024];
    glGetShaderInfoLog( vs, 1024, NULL, info_log );
    std::cout << info_log << "\n";
    return 0;
  }

  GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
  glShaderSource( fs, 1, &fragment_shader_src, NULL );
  glCompileShader( fs );
  glGetShaderiv( fs, GL_COMPILE_STATUS, &success );

  if( !success ) {
    char info_log[1024];
    glGetShaderInfoLog( fs, 1024, NULL, info_log );
    std::cout << info_log << "\n";
    return 0;
  }

  GLuint program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);

  success = GL_FALSE;
  glGetProgramiv( program, GL_LINK_STATUS, &success );

  if( !success ) {
    char info_log[ 1024 ];
    glGetProgramInfoLog( program, 1024, NULL, info_log );
    std::cout << info_log << "\n";
    return 0;
  }

  return program;
}

void setVec3Uniform( const char* uniform_name, vec3& v ) {
  GLint curShader;
  glGetIntegerv( GL_CURRENT_PROGRAM, &curShader );
  GLuint u_loc = glGetUniformLocation( curShader, uniform_name );
  glUniform3fv( u_loc, 1, glm::value_ptr(v));
}

void setMat4Uniform( const char* uniform_name, mat4& m ) {
  GLint curShader;
  glGetIntegerv( GL_CURRENT_PROGRAM, &curShader );
  GLuint u_loc = glGetUniformLocation( curShader, uniform_name );
  glUniformMatrix4fv( u_loc, 1, GL_FALSE, glm::value_ptr(m));
}

void setFloatUniform( const char* uniform_name, float input ) {
  GLint curShader;
  glGetIntegerv( GL_CURRENT_PROGRAM, &curShader );
  GLuint u_loc = glGetUniformLocation( curShader, uniform_name );
  glUniform1f( u_loc, input);
}

float deltatime = 0.0f;
float curTime = 0.0f;
float lastTime = 0.0f;

vec3 camera_pos = vec3(0.0f, 0.0f, 8.0f);
vec3 camera_up = vec3( 0.0f, 1.0f, 0.0f);
vec3 camera_right = vec3( 1.0f, 0.0f, 0.0f );
vec3 camera_direction = vec3( 0.0f, 0.0f, -1.0f );
vec3 camera_rotation = vec3( 0.0f, 0.0f, 0.0f );

void process_camera_inputs( GLFWwindow* window ) {
  if( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )
    camera_pos += camera_direction * deltatime * 5.0f;

  if( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )
    camera_pos -= camera_direction * deltatime * 5.0f;

  if( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS )
    camera_pos -= camera_right * deltatime * 5.0f;

  if( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS )
    camera_pos += camera_right * deltatime * 5.0f;
}

float lastCursorX = 0.0f;
float lastCursorY = 0.0f;
bool first_frame_cursor = true;

void mouse_pos_callback( GLFWwindow* window, double posX, double posY ) {
  if( first_frame_cursor ) {
    lastCursorX = posX;
    lastCursorY = posY;
    first_frame_cursor = false;
  }

  float sensitivity = 0.0025f;
  float offsetX = (posX - lastCursorX) * sensitivity;
  float offsetY = (posY - lastCursorY) * sensitivity;

  lastCursorX = posX;
  lastCursorY = posY;

  camera_rotation.x -= offsetY;
  camera_rotation.y -= offsetX;

  mat4 rot = mat4(1.0f);

  // Rotate yaw
  rot = glm::rotate( rot, camera_rotation.y, vec3( 0.0f, 1.0f, 0.0f ) );
  // Rotate pitch
  rot = glm::rotate( rot, camera_rotation.x, vec3( 1.0f, 0.0f, 0.0f ) );

  camera_right     = glm::normalize( rot * vec4( 1.0f, 0.0f,  0.0f, 0.0f ) );
  camera_up        = glm::normalize( rot * vec4( 0.0f, 1.0f,  0.0f, 0.0f ) );
  camera_direction = glm::normalize( rot * vec4( 0.0f, 0.0f, -1.0f, 0.0f ) );
}

float spherization = 0.0f;

void processInputs( GLFWwindow* window ) {
  if( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) glfwSetWindowShouldClose( window, true );

  // Toggle wireframe mode
  if( glfwGetKey( window, GLFW_KEY_1 ) == GLFW_PRESS ) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  if( glfwGetKey( window, GLFW_KEY_2 ) == GLFW_PRESS ) glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  // Change spherization ratio
  if( glfwGetKey( window, GLFW_KEY_Q ) ) spherization -= deltatime;
  if( glfwGetKey( window, GLFW_KEY_E ) ) spherization += deltatime;
}

typedef struct {
  vec3  pos;
  vec3  color;
  Mesh* mesh;

  void draw() {
    glBindVertexArray( mesh->vao );
    glDrawElements( GL_TRIANGLES, mesh->numTriangles * 3, GL_UNSIGNED_INT, 0 );
    glBindVertexArray( 0 );
  }
} Light;


void render_loop( GLFWwindow* window ) {
  glViewport( 0.0f, 0.0f, SCREEN_W, SCREEN_H );
  glClearColor( 0.1f, 0.1f, 0.2f, 1.0f );
  glEnable( GL_DEPTH_TEST );

  Mesh cube = gen_cube( 32, 32, spherization );
  Mesh sphere = gen_cube( 16, 16, 1.0f );
  
  GLuint common_shader = process_shaders( vs_src, fs_src );
  GLuint light_shader = process_shaders( vs_src, fs_light_src );
  
  std::cout << "Num vertices: " << cube.numVertices << "\n";
  std::cout << "Num triangles: " << cube.numTriangles << "\n";
  std::cout << "program: " << common_shader << "\n";

  mat4 model      = mat4( 1.0f );
  mat4 view       = mat4( 1.0f );
  mat4 projection = glm::perspective( glm::radians( 45.0f ), 1.0f, 0.001f, 2000.0f );

  curTime = glfwGetTime();
  lastTime = glfwGetTime();

  Light light = (Light){
    .pos   = { 4.0f, 0.0f, 0.0f },
    .color = { 1.0f, 0.0f, 1.0f },
    .mesh  = &sphere
  };

  vec3 ambient_light = { 0.2f, 0.2f, 0.2f };

  camera_pos = { 0.0f, 0.0f, 4.0f };

  while ( !glfwWindowShouldClose( window ) ) {
    curTime = glfwGetTime();
    deltatime = curTime - lastTime;
    lastTime = glfwGetTime();

    processInputs( window );
    process_camera_inputs( window );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glUseProgram( common_shader );

    view = glm::lookAt( camera_pos, camera_pos + camera_direction, camera_up );

    model = glm::rotate( model, glm::radians( 1.0f ), vec3( 0.0f, 1.0f, 0.0f ) );

    setMat4Uniform(  "model"       , model        );
    setMat4Uniform(  "view"        , view         );
    setMat4Uniform(  "projection"  , projection   );
    setFloatUniform( "spherization", spherization );
    setFloatUniform( "scale"       , 1.0f         );

    setVec3Uniform( "ambient_light", ambient_light );
    setVec3Uniform( "light_color"  , light.color   );
    setVec3Uniform( "light_pos"    , light.pos     );
    setVec3Uniform( "eye_pos"      , camera_pos    );

    glBindVertexArray( cube.vao );
    glDrawElements( GL_TRIANGLES, cube.numTriangles * 3, GL_UNSIGNED_INT, 0 );

    glUseProgram( light_shader );

    mat4 lightModel = glm::translate( mat4(1.0f), light.pos );

    setMat4Uniform(  "model"       , lightModel   );
    setMat4Uniform(  "view"        , view         );
    setMat4Uniform(  "projection"  , projection   );
    setFloatUniform( "spherization", 1.0f         );
    setFloatUniform( "scale"       , 0.125f       );

    setVec3Uniform( "light_color", light.color );

    light.draw();

    glfwSwapBuffers( window );
    glfwPollEvents();
  }

  free_mesh( cube );
}

void framebuffer_size_callback( GLFWwindow* window, int w, int h ) {
  glViewport( 0, 0, w, h );
}


GLFWwindow* init_engine() {
  glfwInit();

  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

  GLFWwindow* window = glfwCreateWindow( SCREEN_W, SCREEN_H, "Curvature Factor", NULL, NULL );

  if( !window )  return NULL;

  glfwMakeContextCurrent( window );
  glfwSetFramebufferSizeCallback( window, framebuffer_size_callback );
  glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
  glfwSetCursorPosCallback( window, mouse_pos_callback );

  if( !gladLoadGLLoader((GLADloadproc) glfwGetProcAddress) ) return NULL;

  return window;
}

void close_engine( GLFWwindow* window ) {
  if( window ) glfwDestroyWindow( window );
  glfwTerminate();
}

int main() {
  GLFWwindow* window = init_engine();
  render_loop( window );
  close_engine( window );
  return 0;
}
