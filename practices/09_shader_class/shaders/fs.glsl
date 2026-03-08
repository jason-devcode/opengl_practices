#version 330 core

in vec3 vColor;
out vec4 FragColor;

uniform float brightness;

void main() {
    FragColor = vec4(vColor, 1.0) * brightness;
}