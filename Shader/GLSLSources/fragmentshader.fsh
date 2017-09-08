#version 310 es

precision mediump float;
out vec4 FragColor;
in vec2 atexCoord;
in vec2 atexCoord2;
in vec2 atexCoord3;
in vec3 Normal;
in vec3 fragPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

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

    if(isTex==1 && (atexCoord.x !=0.0f && atexCoord.y != 0.0f)){
            vec4 color = mix(texture(texture2, atexCoord2),texture(texture1, atexCoord), 0.3f);
            FragColor = mix(color, texture(texture3, atexCoord3), 0.5f);
            FragColor = texture(texture1, atexCoord);
    } else {
        FragColor = vec4(result, 1);
    }

    //FragColor = vec4(0.7f, 0.7f, 0.7f, 1.0f);
}
