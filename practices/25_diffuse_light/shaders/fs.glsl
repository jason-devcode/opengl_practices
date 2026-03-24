#version 330 core

in vec2 TexCoord;
in vec3 vColor;

in vec3 pixelPos;
in vec3 vNormal;

out vec4 FragColor;

uniform sampler2D tex0;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambientLight;

void main() {
    vec3 lightDir = normalize( lightPos - pixelPos );
    vec3 diffuseLight = lightColor * max( dot( vNormal, lightDir ), 0.0 );

    FragColor = texture( tex0, TexCoord ) * vec4( ambientLight + diffuseLight, 1.0 );
    // FragColor = texture( tex0, TexCoord ) * vec4( lightColor, 1.0 );
}