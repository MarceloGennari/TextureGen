#version 310 es

precision mediump float;
out vec4 FragColor;

in vec2 atexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, atexCoord);
}
