#version 330 core

in vec2 TexCoord;
in vec3 vColor;

in vec3 fragPos;
in vec3 vNormal;

out vec4 FragColor;

uniform sampler2D tex0;

uniform float specIntensity;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambientLight;


void main() {
    vec3 lightDir = normalize( lightPos - fragPos );
    vec3 viewDir = normalize( viewPos - fragPos );

    float diff = max( dot( vNormal, lightDir ), 0.0 );
    vec3 diffuse = lightColor * diff;

    // Calculate specular light
    vec3 reflect_light = normalize( reflect( -lightDir, vNormal ) );
    float spec = pow( max( dot( viewDir, reflect_light ), 0 ), specIntensity );

    vec3 specular = lightColor * spec ;

    FragColor = texture( tex0, TexCoord ) * vec4( ambientLight + diffuse + specular, 1.0 );
}