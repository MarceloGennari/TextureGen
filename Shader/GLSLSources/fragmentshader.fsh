#version 410 core

//precision mediump float;
out vec4 FragColor;
in vec2 atexCoord;
uniform sampler2D texture1;


void main()
{
    //FragColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
    FragColor = texture(texture1, atexCoord);
}
