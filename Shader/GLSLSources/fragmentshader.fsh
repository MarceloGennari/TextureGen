#version 410 core
out vec4 FragColor;

in vec2 atexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, atexCoord);
}
