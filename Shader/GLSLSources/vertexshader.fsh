#version 310 es

precision mediump float;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec2 texCoord2;
layout(location = 4) in vec2 texCoord3;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 atexCoord;
out vec2 atexCoord2;
out vec2 atexCoord3;
out vec3 Normal;
out vec3 fragPos;

void main()
{
    gl_Position = projection*view*model*vec4(aPos, 1.0);
    atexCoord = vec2(texCoord.x, texCoord.y);
    atexCoord2 = vec2(texCoord2.x, texCoord2.y);
    atexCoord3 = vec2(texCoord3.x, texCoord3.y);
    fragPos = vec3(model*vec4(aPos,1.0));

    Normal = normal;
}
