#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include "glm/common.hpp"
#include "shader.h"
#include "GL/glew.h"
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "objects.h"


class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureS> textures);
    Mesh(float *points, int size1, bool Texture, float *indices, int size2);
    void Draw(Shader shader);

    std::vector<TextureS> textures;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

private:
    unsigned int VAO, VBO, EBO;
    void setUpMesh();

    std::vector<TextureS> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif // MESH_H
