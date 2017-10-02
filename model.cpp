#include "model.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <parse_stl.h>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

Model* Model::m;

Model::Model(){}

void Model::Draw(Shader shader){
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void Model::loadModel(std::string path){

    directory = path.substr(0, path.find_last_of('/'));

    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    stl::parse_stl("/home/marcelo/TextureGen/Teddy/mesh.stl", vertices, indices);

    /* WHOLE ALGORITHM */
    TextureEngine::SurfaceSimplificationEngine::unRepVert(vertices, indices);

    // This is going to hold a list of all of the faces in the model
    std::vector<int> faces;

    for(int k = 0; k<vertices.size(); k++){
        // Remember that each vertex has a vector of faces that it belongs to
        faces.insert(faces.end(), vertices[k].faces.begin(), vertices[k].faces.end());
    }
    std::sort(faces.begin(), faces.end());
    faces.erase(std::unique(faces.begin(), faces.end()), faces.end());

    /* This part loads the Textures from the sample keys
     * */
    // Getting the Key Frames
    std::vector<Frame *> frames = TextureEngine::SaptiotemporalEngine::temporalSampling(30,50,504);

    for(int k = 0; k<frames.size(); k++){
        TextureEngine::TextureMapGenEngine::getTextureCoords(vertices, indices, frames[k], k, frames.size());
    }

    Texture texture;
    int textureId = TextureFromFile(frames);
    texture.id = textureId;
    texture.type = "texture_diffuse";
    textures.push_back(texture);
    Mesh meshT = Mesh(vertices,indices,textures);
    meshes.push_back(meshT);
}

unsigned int Model::TextureFromFile(std::vector<Frame *> frames)
{
    glewInit();

    int width = frames[0]->frame->getWidth(); int height = frames[0]->frame->getHeight(); int nrComponents =3;
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int NrFrames = frames.size();

    unsigned char* data = frames[0]->frame->getRGB();
    int sizeData = width*height*nrComponents;

    unsigned char* result = new unsigned char[width*height*nrComponents*NrFrames];
    int sizeResult = 0;

    std::copy(data, data+sizeData, result);
    sizeResult = sizeResult + sizeData;

    free(data);

    for(int k = 1; k<NrFrames; k++){

        int sizeData2 = width*height*nrComponents;
        unsigned char *data2 = new unsigned char[sizeData2];
        data2 = frames[k]->frame->getRGB();

        std::copy(data2, data2+sizeData2, result+sizeResult);
        sizeResult = sizeResult+sizeData2;
        free(data2);
    }


    if (result)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height*NrFrames, 0, GL_RGB, GL_UNSIGNED_BYTE, result);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);

        free(result);
    }
    else
    {
        std::cout << "Texture failed to load" << std::endl;
    }

    return textureID;
}
