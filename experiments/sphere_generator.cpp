#include <iostream>
#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define INITIAL_WIDTH 1024
#define INITIAL_HEIGHT 1024
#define WINDOW_TITLE "Sphere Generator"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

using u8  = std::uint8_t ;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

const char* vertexShader = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 vNormal;
out vec3 FragPos;

// MVP 3D Model
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  TexCoord = aTexCoord;
  vNormal  = aNormal;
  FragPos  = aPos;
  gl_Position = projection * view * model * vec4( aPos, 1.0 );
}
)";

const char* fragmentShader = R"(
#version 330 core

in vec2 TexCoord;
in vec3 vNormal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D tx0;
uniform bool bWireframeMode;

uniform vec3 camera_pos;

uniform vec3 light_pos;
uniform vec3 light_color;

void main() {
  if( bWireframeMode ) {
    FragColor = vec4( 1.0 );
    return;
  }

  vec3 ambient_color = vec3( 0.2, 0.2, 0.2 );
  
  vec3 light_dir = normalize( light_pos - FragPos );
  vec3 view_dir = normalize( camera_pos - FragPos );

  float diff = max( dot( light_dir, vNormal ), 0.0 );
  vec3 diffuse = diff * light_color;

  vec3 reflect_dir = normalize(reflect( -light_dir, vNormal ));

  float spec = dot( reflect_dir, view_dir );
  vec3 specular = light_color * pow( max( spec, 0.0 ), 8 ); 

  vec4 txcolor = texture( tx0, TexCoord ); 

  if(txcolor.r == 0) FragColor = txcolor + vec4( specular,1.0);
  else FragColor = txcolor * vec4( diffuse + specular + ambient_color, 1.0 );
}
)";

const char* lightFragmentShader = R"(
#version 330 core

uniform vec3 light_color;

out vec4 FragColor;

void main() {
  FragColor = vec4(light_color, 1.0);
}
)";


GLuint process_shaders( const char* vs_src, const char* fs_src ) {
  GLuint vs = glCreateShader( GL_VERTEX_SHADER );
  glShaderSource( vs, 1, &vs_src, NULL);
  glCompileShader( vs );

  GLint success;
  glGetShaderiv( vs, GL_COMPILE_STATUS, &success );

  if( !success ) {
    char info_log[ 1024 ];
    glGetShaderInfoLog( vs, 1024, NULL, info_log );
    std::cerr << info_log << "\n";
    return 0;
  }

  GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
  glShaderSource( fs, 1, &fs_src, NULL);
  glCompileShader( fs );

  glGetShaderiv( fs, GL_COMPILE_STATUS, &success );

  if( !success ) {
    char info_log[ 1024 ];
    glGetShaderInfoLog( fs, 1024, NULL, info_log );
    std::cerr << info_log << "\n";
    return 0;
  }

  GLuint program = glCreateProgram();
  glAttachShader( program, vs );
  glAttachShader( program, fs );
  glLinkProgram( program );

  glGetProgramiv( program, GL_LINK_STATUS, &success );

  if( !success ) {
    char info_log[ 1024 ];
    glGetProgramInfoLog( fs, 1024, NULL, info_log );
    std::cerr << info_log << "\n";
    return 0;
  }
  return program;
}

typedef struct {
  vec3 pos;
  vec3 normal;
  vec2 txcoord;
} Vertex;

typedef struct {
  int A, B, C;
} Triangle;

typedef struct {
  int numVertices;
  int numTriangles;
  GLuint vao;
  Vertex* vertices = NULL;
  Triangle* triangles = NULL;

  void clean() {
    if(vertices) delete[] vertices;
    if(triangles) delete[] triangles;
    glDeleteVertexArrays( 1, &vao );
    vao = 0;
    numTriangles = 0;
    numVertices = 0;
  }

  void draw( GLuint texture ) {
    glBindVertexArray( vao );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, texture );
    glDrawElements( GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_INT, 0 );
  }
} Mesh;

GLuint build_vao( Vertex* vertices, Triangle* triangles, int numVertices, int numTriangles ) {
  GLuint vao;
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );

  // Set vbo
  GLuint vbo;
  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertices, GL_STATIC_DRAW );

  // Set Attribute pointers
  // pos
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof( Vertex, pos ) );
  glEnableVertexAttribArray( 0 );

  // normal
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof( Vertex, normal ) );
  glEnableVertexAttribArray( 1 );

  // txcoord
  glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof( Vertex, txcoord ) );
  glEnableVertexAttribArray( 2 );

  // Set EBO
  GLuint ebo;
  glGenBuffers( 1, &ebo );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle) * numTriangles, triangles, GL_STATIC_DRAW );

  glBindVertexArray(0);
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

  return vao;
}

Mesh gen_quad() {
  Vertex vertices[] = {
    { .pos = { -1.0,  1.0, 0.0 }, .normal = { 0.0, 0.0, 1.0 }, .txcoord = { 0.0, 0.0 } }, 
    { .pos = {  1.0,  1.0, 0.0 }, .normal = { 0.0, 0.0, 1.0 }, .txcoord = { 1.0, 0.0 } }, 
    { .pos = {  1.0, -1.0, 0.0 }, .normal = { 0.0, 0.0, 1.0 }, .txcoord = { 1.0, 1.0 } }, 
    { .pos = { -1.0, -1.0, 0.0 }, .normal = { 0.0, 0.0, 1.0 }, .txcoord = { 0.0, 1.0 } }, 
  };

  Triangle triangles[] = {
    { 3, 2, 1 },
    { 3, 1, 0 }
  };

  int numVertices  = sizeof( vertices ) / sizeof(Vertex);
  int numTriangles = sizeof(triangles) / sizeof( Triangle );

  GLuint vao = build_vao( vertices, triangles, numVertices, numTriangles );

  return (Mesh){ .numVertices = numVertices, .numTriangles = numTriangles, .vao = vao };
}

const float PI = 3.14159265f;

Mesh gen_sphere( int meridians, int parallels, float radius ) {
  int numVertices = meridians * parallels;

  Vertex *vertices = new Vertex[ numVertices ];

  // Generate vertices
  for( int parallel = 0; parallel < parallels; parallel++ ) {
    for( int meridian = 0; meridian < meridians; meridian++ ) {
      int vertexIndex = parallel * meridians + meridian;

      float u = (float) meridian / (float) (meridians - 1);
      float v = (float) parallel / (float) (parallels - 1);
      float lenght   = u * 2.0f * PI;
      float latitude = v * PI;

      vec3 n = {
        cos( lenght ) * sin( latitude ),
        cos( latitude ),
        sin( lenght ) * sin( latitude )
      };

      vertices[ vertexIndex ] = (Vertex){
        .pos = n * radius,
        .normal = n,
        .txcoord = { u, v }
      };
    }
  }

  int numTriangles = ( meridians - 1 ) * ( parallels - 1 ) * 2; // 2 triangles per quad

  Triangle *triangles = new Triangle[ numTriangles ];

  for( int j = 0; j < parallels - 1; j++ ) {
    for( int i = 0; i < meridians - 1; i++ ) {
      int triangleIndex = j * (meridians - 1) * 2 + i * 2;

      int indexA = j * meridians + i;
      int indexB = j * meridians + (i + 1);
      int indexC = (j + 1) * meridians + (i + 1);
      int indexD = (j + 1) * meridians + i;

      triangles[ triangleIndex + 0 ] = { indexA, indexB, indexC };
      triangles[ triangleIndex + 1 ] = { indexA, indexC, indexD };
    }
  }

  GLuint vao = build_vao( vertices, triangles, numVertices, numTriangles );

  return (Mesh){ .numVertices = numVertices, .numTriangles = numTriangles, .vao = vao, .vertices = vertices, .triangles = triangles };
}

typedef struct {
  u32 width;
  u32 height;
  u32 nrChannels;
  u32 *data;
  GLuint txobj;
} Texture;

Texture gen_chess_texture( u32 tile_w, u32 tile_h ) {
  const int default_w = 512;
  const int default_h = 512;

  // First generate texture
  Texture tx;
  tx.data = new u32[ default_w * default_h ];

  // Gen chess pattern
  for( int Y = 0; Y < default_h; Y++ ) {
    for( int X = 0; X < default_w; X++ ) {
      tx.data[ Y * default_w + X ] = (((X & tile_w) ^ (Y & tile_h)) * 0x00FFFFFF) | 0xFF000000;
    }
  }

  tx.width = default_w;
  tx.height = default_h;
  tx.nrChannels = 4;

  glGenTextures( 1, &tx.txobj );
  glBindTexture( GL_TEXTURE_2D, tx.txobj );

  // Set texture wrapping
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

  // Set texture filtering
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, default_w, default_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tx.data );

  glGenerateMipmap( GL_TEXTURE_2D );
  glBindTexture( GL_TEXTURE_2D, 0 );

  return tx;
}

void setInt( const char* uniform, int input ) {
  GLint currShader; 
  glGetIntegerv( GL_CURRENT_PROGRAM, &currShader );
  GLuint u_loc = glGetUniformLocation( currShader, uniform );
  glUniform1i( u_loc, input );
}

void setBool( const char* uniform, bool input ) {
  GLint currShader; 
  glGetIntegerv( GL_CURRENT_PROGRAM, &currShader );
  GLuint u_loc = glGetUniformLocation( currShader, uniform );
  glUniform1i( u_loc, input );
}

void setFloat( const char* uniform, float input ) {
  GLint currShader; 
  glGetIntegerv( GL_CURRENT_PROGRAM, &currShader );
  GLuint u_loc = glGetUniformLocation( currShader, uniform );
  glUniform1f( u_loc, input );
}

void setVec3( const char* uniform, vec3& input ) {
  GLint currShader; 
  glGetIntegerv( GL_CURRENT_PROGRAM, &currShader );
  GLuint u_loc = glGetUniformLocation( currShader, uniform );
  glUniform3fv( u_loc, 1, glm::value_ptr( input ) );
}

void setMat4( const char* uniform, mat4& input ) {
  GLint currShader; 
  glGetIntegerv( GL_CURRENT_PROGRAM, &currShader );
  GLuint u_loc = glGetUniformLocation( currShader, uniform );
  glUniformMatrix4fv( u_loc, 1, GL_FALSE, glm::value_ptr( input ) );
}

bool bDrawWireframeMode = false;
bool bToggleWireframeKeyPress = false;

void process_inputs( GLFWwindow* window ) {
  // Process Key Escape
  if( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) glfwSetWindowShouldClose( window, true );

  // Toggle wireframe mode
  if( glfwGetKey( window, GLFW_KEY_1 ) == GLFW_PRESS ) {
    if( !bToggleWireframeKeyPress ) {
      bToggleWireframeKeyPress = true;
      bDrawWireframeMode = !bDrawWireframeMode;

      if( bDrawWireframeMode ) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      else glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
  } else {
    bToggleWireframeKeyPress = false;
  }
}

float deltatime = 0.0f;
float last_time = 0.0f;
float curr_time = 0.0f;

vec3 camera_pos      = { 0.0f, 0.0f, 16.0f };
vec3 camera_rotation = { 0.0f, 0.0f, 0.0f };
vec3 camera_right    = { 1.0f, 0.0f, 0.0f };
vec3 camera_up       = { 0.0f, 1.0f, 0.0f };
vec3 camera_front    = { 0.0f, 0.0f, -1.0f };

float camera_speed = 4.0f;

float cursor_sensitivity = 0.001f;
float last_cursor_x = 0.0f;
float last_cursor_y = 0.0f;
bool first_cursor_update = true;

bool toggle_camera_free_pressed = false;
bool enable_free_camera = true;

inline void update_deltatime() {
  curr_time = glfwGetTime();
  deltatime = curr_time - last_time;
  last_time = curr_time;
}

void process_camera_inputs( GLFWwindow* window ) {
  /// Move camera forward
  if( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS ) camera_pos += camera_front * camera_speed * deltatime;
  if( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS ) camera_pos -= camera_front * camera_speed * deltatime;

  /// Move camera sideway
  if( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS ) camera_pos -= camera_right * camera_speed * deltatime;
  if( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS ) camera_pos += camera_right * camera_speed * deltatime;

  if( glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS ) {
    if( !toggle_camera_free_pressed ) {
      toggle_camera_free_pressed = true;
      enable_free_camera = !enable_free_camera;
      if( enable_free_camera ) glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
      else glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    }
  } else {
    toggle_camera_free_pressed = false;
  }
}

void cursor_pos_callback( GLFWwindow*, double posX, double posY ) {
  if( first_cursor_update ) {
    last_cursor_x = posX;
    last_cursor_y = posY;
    first_cursor_update = false;
  }
  if( enable_free_camera ) {
    last_cursor_x = INITIAL_WIDTH / 2;
    last_cursor_y = INITIAL_HEIGHT / 2;
    return;
  }

  float offsetX = posX - last_cursor_x;
  float offsetY = posY - last_cursor_y;

  // Update Yaw Angle
  camera_rotation.y -= offsetX * cursor_sensitivity;
  // Update Pitch Angle
  camera_rotation.x -= offsetY * cursor_sensitivity;

  mat4 view = mat4(1.0); 
  view = glm::rotate( view, camera_rotation.y, vec3( 0.0f, 1.0f, 0.0f ) ); // Yaw rotation
  view = glm::rotate( view, camera_rotation.x, vec3( 1.0f, 0.0f, 0.0f ) ); // Pitch rotation

  // Update right, up, and front camera axis
  camera_right = view * vec4( 1.0f, 0.0f, 0.0f, 0.0f );
  camera_up    = view * vec4( 0.0f, 1.0f, 0.0f, 0.0f );
  camera_front = view * vec4( 0.0f, 0.0f, -1.0f, 0.0f );

  last_cursor_x = posX;
  last_cursor_y = posY;
}

typedef struct {
  vec3 pos;
  vec3 color;
} Light;


void render_loop( GLFWwindow* window ) {
  glViewport( 0, 0, INITIAL_WIDTH, INITIAL_HEIGHT );
  glClearColor( 0.0, 0.0, 0.0, 1.0 );
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_CULL_FACE );

  GLuint common_shader = process_shaders( vertexShader, fragmentShader      );
  GLuint light_shader  = process_shaders( vertexShader, lightFragmentShader );

  Mesh quad = gen_quad();
  Mesh sphere = gen_sphere( 256, 256, 4.0f );
Texture chess_tx = gen_chess_texture( 32, 32 );

  mat4 model = mat4(1.0f);
  mat4 view  = mat4(1.0f);

  mat4 projection = glm::perspective( glm::radians( 45.0f ), (float)(INITIAL_WIDTH / INITIAL_HEIGHT), 0.01f, 3000.0f );

  curr_time = glfwGetTime();
  last_time = glfwGetTime();

  Light light;
  light.pos = { 12.0f, 0.0f, 0.0f };
  light.color = { 1.0f, 1.0f, 1.0f };

  while( !glfwWindowShouldClose( window ) ) {
    update_deltatime();
    process_inputs( window );
    process_camera_inputs( window );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram( common_shader );
    setBool( "bWireframeMode", bDrawWireframeMode );

    view = glm::lookAt( camera_pos, camera_pos + camera_front, camera_up );

    setMat4( "model"      , model       );
    setMat4( "view"       , view        );
    setMat4( "projection" , projection  );
    setVec3( "light_color", light.color );
    setVec3( "light_pos"  , light.pos   );
    setVec3( "camera_pos" , camera_pos  );

    sphere.draw( chess_tx.txobj );

    glUseProgram( light_shader );
    mat4 light_model = glm::scale( glm::translate( mat4(1.0f), light.pos ), vec3( 0.1f ) );
    setMat4( "model"     , light_model );
    setMat4( "view"      , view        );
    setMat4( "projection", projection  );
    setVec3( "light_color", light.color );
    sphere.draw( 0 );

    glfwSwapBuffers( window );
    glfwPollEvents();
  }

  quad.clean();
  sphere.clean();
  delete[] chess_tx.data;
}

void framebuffer_size_callback( GLFWwindow*, int w, int h ) {
  glViewport( 0, 0, w, h );
}

GLFWwindow* init_engine() {
  glfwInit();

  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

  GLFWwindow* window = glfwCreateWindow( INITIAL_WIDTH, INITIAL_HEIGHT, WINDOW_TITLE, NULL, NULL );

  if( !window ) {
    return NULL;
  }

  glfwMakeContextCurrent( window );

  if( !gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress ) ) {
    glfwDestroyWindow( window );
    return NULL;
  }

  glfwSetFramebufferSizeCallback( window, framebuffer_size_callback );
  glfwSetCursorPosCallback( window, cursor_pos_callback );
  glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );

  return window;
}

void close_engine( GLFWwindow* window ) {
  if( window ) glfwDestroyWindow( window );
  glfwTerminate();
}


int main() {
  GLFWwindow* window = init_engine();
  if( !window ) return -1;
  render_loop( window ); 
  close_engine( window );
  return 0;
}

