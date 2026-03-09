#version 330 core

in vec2 TexCoord;
in vec3 vColor;
out vec4 FragColor;

uniform sampler2D texture0;

void main() {
    FragColor = texture( texture0, TexCoord ) * vec4(vColor, 1.0);
}