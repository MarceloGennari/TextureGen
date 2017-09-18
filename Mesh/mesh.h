#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include "glm/common.hpp"
#include "shader.h"
#include "GL/glew.h"
#include <sstream>
#include "objects.h"

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader shader);

    std::vector<Texture> textures;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

private:
    unsigned int VAO, VBO, EBO;
    void setUpMesh();
};

#endif // MESH_H
