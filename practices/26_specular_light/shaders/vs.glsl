#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec2 TexCoord;
out vec3 vColor;
out vec3 fragPos;
out vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoord = aTexCoord;
    fragPos = aPos;
    vColor   = aColor;
    vNormal  = aNormal;
    gl_Position = projection * view * model * vec4( aPos, 1.0 );
}