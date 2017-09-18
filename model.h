#ifndef MODEL_H
#define MODEL_H
#include <string>
#include "shader.h"
#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"
#include "textureengine.h"
#include "objects.h"

class Model
{
public:
     /*  Functions   */
     Model();
     static Model* getModel(){
         if(m == NULL){
             m = new Model();
         }
         return m;
     }
     void Draw(Shader shader);
     void loadModel(std::string path);
     std::vector<Mesh> * getMeshes(){return &meshes;}

 private:
     static Model* m;

     /*  Model Data  */
     std::vector<Mesh> meshes;
     std::string directory;
     std::vector<Texture> textures_loaded;

     /*  Functions   */
     void processNode(aiNode *node, const aiScene *scene);
     Mesh processMesh(aiMesh *mesh, const aiScene *scene);
     unsigned int TextureFromFile(std::vector<aiString> path, const std::string &directory);
};

#endif // MODEL_H
