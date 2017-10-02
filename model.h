#ifndef MODEL_H
#define MODEL_H
#include <string>
#include "shader.h"
#include "mesh.h"
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
     unsigned int TextureFromFile(std::vector<Frame *> frames);
};

#endif // MODEL_H
