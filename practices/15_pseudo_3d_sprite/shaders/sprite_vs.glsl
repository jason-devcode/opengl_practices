#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform float yOffset;

void main() {
    TexCoord = aTexCoord;
    float scale = 0.18f;
    gl_Position = vec4( aPos * vec3( scale, scale , 1.0 ) - vec3( 0.0, 0.4 - yOffset, 0.0 ), 1.0 );
}
