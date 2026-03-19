#define STB_IMAGE_IMPLEMENTATION
#define LOG_USE_COLOR

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#include "stb_image.h"
#include "logger.h"

typedef struct {
  int width, height, nrChannels;
  uint8_t* data;
  GLuint obj;
  GLboolean success;
} Texture;

Texture load_texture( const char* filepath ) {
  LOG_INFO( "Loading texture: %s...", filepath );

  int width, height, nrChannels;

  uint8_t* data = NULL;
  
  data = stbi_load( filepath, &width, &height, &nrChannels, 0);

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
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

  // Set texture filter
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  // Set texture data
  if( nrChannels == 3 )
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
  else
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
  
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

#endif