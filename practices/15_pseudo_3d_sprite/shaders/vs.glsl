#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aIndex;

noperspective out vec3 vTexCoord;

uniform vec2 uvFar1, uvFar2, uvNear1, uvNear2;
uniform float uNear; 
uniform float uFar; 

uniform int uSky;

void main() {
    vec2 uv;
    float w;

    if (aIndex.x == 1.0) { uv = uvNear1; w = uNear; }
    if (aIndex.x == 2.0) { uv = uvNear2; w = uNear; }
    if (aIndex.x == 3.0) { uv = uvFar1;  w = uFar;  }
    if (aIndex.x == 4.0) { uv = uvFar2;  w = uFar;  }

    // Coordenadas homogéneas para interpolación perspectiva manual
    vTexCoord = vec3(uv / w, 1.0 / w);

    if( uSky == 0 ) gl_Position = vec4(aPos * vec3( 1.0, 0.5, 1.0 ) + vec3(0.0, -0.5, 0.0), 1.0);
    else gl_Position = vec4(aPos * vec3( 1.0, -0.5, 1.0 ) + vec3(0.0, 0.5, 0.0), 1.0);
}