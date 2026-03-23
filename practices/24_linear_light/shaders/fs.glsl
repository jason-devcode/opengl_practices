#version 330 core

in vec2 TexCoord;
in vec3 vColor;

in vec3 pixelPos;

out vec4 FragColor;

uniform sampler2D tex0;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main() {
    float d = distance( lightPos, pixelPos );

    float intensity = 1 - pow( (d - 8) / 10.0, 1 );

    // FragColor = vec4( vec3( intensity ), 1.0 );
    // FragColor = vec4( pixelPos, 1.0 );    
    //FragColor = texture( tex0, TexCoord ) * intensity;
    FragColor = texture( tex0, TexCoord ) * vec4( vColor * lightColor * intensity, 1.0);
}