#ifndef MODEL_H
#define MODEL_H
#include <string>
#include "shader.h"
#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"

class Model
{
public:
     /*  Functions   */
     Model(char *path)
     {
         loadModel(path);
     }
     void Draw(Shader shader);
 private:
     /*  Model Data  */
     std::vector<Mesh> meshes;
     std::string directory;
     std::vector<TextureS> textures_loaded;

     /*  Functions   */
     void loadModel(std::string path);
     void processNode(aiNode *node, const aiScene *scene);
     Mesh processMesh(aiMesh *mesh, const aiScene *scene);
     std::vector<TextureS> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                          std::string typeName);
     unsigned int TextureFromFile(const char *path, const std::string &directory);
};

#endif // MODEL_H
