#ifndef LIGHT_H
#define LIGHT_H
#include <glm/common.hpp>
#include "shader.h"

class Light
{
public:
    Light(glm::vec3 Pos, glm::vec3 colour);
    void Draw();

    glm::vec3 getPos();
    glm::vec3 getColor();

private:
    glm::vec3 Pos;
    glm::vec3 colour;
    Shader *sh;
    float points[108];
};

#endif // LIGHT_H
