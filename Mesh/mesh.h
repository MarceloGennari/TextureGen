#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include "glm/common.hpp"
#include "shader.h"
#include "GL/glew.h"
#include <sstream>

struct Vertex{
    glm::vec3 Pos;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct TextureS{
    unsigned int id;
    std::string type;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureS> textures);
    Mesh(float points[], bool Texture, float indices[]);
    void Draw(Shader shader);

private:
    unsigned int VAO, VBO, EBO;
    void setUpMesh();

    std::vector<TextureS> textures;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

#endif // MESH_H
