#version 310 es

precision mediump float;
out vec4 FragColor;
in vec2 atexCoord;
in vec3 Normal;
in vec3 fragPos;

uniform sampler2D arrayTexture;

uniform int isTex;

uniform vec3 lightColour;
uniform vec3 objectColour;
uniform vec3 lightPos;

void main()
{
    // Ambient Light
    float ambientStrength = 0.1;
    vec3 ambient = lightColour*ambientStrength;

    // Diffuse Light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos-fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff*lightColour;

    vec3 result = (ambient+diffuse)*objectColour;
    result = vec3(1.0f, 1.0f, 1.0f);

    if(isTex==1 && (atexCoord.x !=0.0f && atexCoord.y != 0.0f && atexCoord.x<1.0f && atexCoord.y<1.0f)){
            FragColor = texture(arrayTexture, vec2(atexCoord));
    } else {
        FragColor = vec4(result, 1);
    }
}
