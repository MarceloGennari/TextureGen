#ifndef MODEL_H
#define MODEL_H
#include <string>
#include "shader.h"
#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"

struct Pair{
    std::pair<unsigned int, unsigned int> vecPair;
    Vertex contVert;
    float cost;
};

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
 private:
     static Model* m;

     /*  Model Data  */
     std::vector<Mesh> meshes;
     std::string directory;
     std::vector<TextureS> textures_loaded;

     /*  Functions   */
     void processNode(aiNode *node, const aiScene *scene);
     Mesh processMesh(aiMesh *mesh, const aiScene *scene);
     std::vector<TextureS> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                          std::string typeName);
     unsigned int TextureFromFile(const char *path, const std::string &directory);
     std::vector<std::pair<unsigned int, unsigned int> > PairSelection(std::vector<unsigned int> &ind);
     void unRepVert(std::vector<Vertex> &vs, std::vector<unsigned int> &ind);
     void Optimization(std::vector<Vertex> &vs, std::vector<unsigned int> &ind);
};

#endif // MODEL_H
