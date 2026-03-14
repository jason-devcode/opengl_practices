#version 330 core

in vec2 TexCoord;
in vec3 vColor;
out vec4 FragColor;

uniform sampler2D tex0;
uniform sampler2D tex1;

void main() {
    FragColor = texture( tex0, TexCoord );
}