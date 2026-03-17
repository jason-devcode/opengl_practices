#version 330 core

noperspective in vec3 vTexCoord;
out vec4 FragColor;
uniform sampler2D tex0;

void main() {
    // División perspectiva: recupera uv correcto
    vec2 uv = vTexCoord.xy / vTexCoord.z;
    FragColor = texture(tex0, uv);
}