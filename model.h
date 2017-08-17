#ifndef MODEL_H
#define MODEL_H
#include <string>
#include "shader.h"
#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
     /*  Functions   */
     void loadModel(std::string path);
     void processNode(aiNode *node, const aiScene *scene);
     Mesh processMesh(aiMesh *mesh, const aiScene *scene);
     std::vector<TextureS> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                          std::string typeName);
};

#endif // MODEL_H
