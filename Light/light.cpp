#include "light.h"
#include "GL/glew.h"

Light::Light(glm::vec3 Pos, glm::vec3 colour)
{
    float p[] = {
        -0.5f, -0.5f, -0.5f, // point 0
         0.5f, -0.5f, -0.5f, // point 1
         0.5f,  0.5f, -0.5f, // point 2
         0.5f,  0.5f, -0.5f, // point 2
        -0.5f,  0.5f, -0.5f, // point 3
        -0.5f, -0.5f, -0.5f, // point 0

        -0.5f, -0.5f,  0.5f, // point 4
         0.5f, -0.5f,  0.5f, // point 5
         0.5f,  0.5f,  0.5f, // point 6
         0.5f,  0.5f,  0.5f, // point 6
        -0.5f,  0.5f,  0.5f,// point 7
        -0.5f, -0.5f,  0.5f,// point 4

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f};

    this->Pos = Pos;
    this->colour = colour;
    this->sh = new Shader("/home/marcelo/TextureGen/Light/lightvs.vert","/home/marcelo/TextureGen/Light/lightfs.frag");
}

glm::vec3 Light::getColor(){
    return colour;
}

glm::vec3 Light::getPos(){
    return Pos;
}

void Light::Draw(){
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}
