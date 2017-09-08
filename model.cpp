#include "model.h"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()


Model* Model::m;

Model::Model(){}

void Model::Draw(Shader shader){
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);

}

void Model::loadModel(std::string path){
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureS> textures;

    // Walk through each of the mesh's vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Pos = vector;
        // normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;



        // texture coordinates
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
        }
        else{
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    /* WHOLE ALGORITHM */

    TextureEngine::SurfaceSimplificationEngine::Optimize2(vertices, indices);

    /* This part loads the Textures from the sample keys
     * */
    // Getting the Key Frames
    std::vector<Frame *> frames = TextureEngine::SaptiotemporalEngine::temporalSampling(30,10,504);

    glewInit();
    GLuint arrayTexture;
    glGenTextures(1,&arrayTexture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, arrayTexture);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 7, GL_RGB, 640, 480, 20);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for(int k = 0; k<frames.size(); k++){
        int frameInd = k;
        TextureEngine::TextureMapGenEngine::getTextureCoords(vertices, indices, frames[k], k, 1);
        std::string nr = SSTR(frames[frameInd]->frameNr);
        TextureS texture;
        aiString str("Frames/0"+nr+".ppm");
        TextureFromFile3(str.C_Str(), this->directory, arrayTexture, k);
        texture.id = arrayTexture;
        texture.type = "texture_diffuse";
        textures.push_back(texture);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, arrayTexture);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    /*
     * The idea of this part is to load all of the textures from the SpatiotemporalEngine in a single texture
     * using that texture we then reconstruct the scene
     * */
//    std::vector<aiString> st;
//    for(int k = 0; k < frames.size(); k++){
//        std::string s;
//        int frameNr = frames[k]->frameNr;
//        if(frameNr<10)
//            s ="Frames/000"+SSTR(frameNr)+".ppm";
//        else if(frameNr<100)
//            s = "Frames/00" + SSTR(frameNr) + ".ppm";
//        else if (frameNr<1000)
//            s = "Frames/0" + SSTR(frameNr) + ".ppm";

//        aiString y(s);
//        st.push_back(y);
//    }

//    for(int k = 0; k <frames.size(); k+=6)
//        TextureEngine::TextureMapGenEngine::getTextureCoords(vertices, indices, frames[k], k, frames.size());

//    TextureS texture;
//    texture.id = TextureFromFile(st, this->directory);
//    texture.type = "texture_diffuse";
//    textures.push_back(texture);

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures);
}

unsigned int Model::TextureFromFile(std::vector<aiString> path, const std::string &directory)
{
    glewInit();

    /* This doesn't work because of blending problems.
     * When blending in a same texture, the OpenGL framework will create a "shading" from the position to the destination.
     * In this case, it will be a "jump" and it will make the transition look awful
     * */

    int width, height, nrComponents;
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int NrFrames = path.size();

    std::string filename = std::string(path[0].C_Str());
    filename = directory + '/' + filename;

    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    int sizeData = width*height*nrComponents;

    unsigned char* result = new unsigned char[width*height*nrComponents*NrFrames];
    int sizeResult = 0;

    std::copy(data, data+sizeData, result);
    sizeResult = sizeResult + sizeData;

    for(int k = 1; k<NrFrames; k++){
        filename = std::string(path[k].C_Str());
        filename = directory + '/' + filename;

        int sizeData2 = width*height*nrComponents;
        unsigned char *data2 = new unsigned char[sizeData2];
        data2 = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

        std::copy(data2, data2+sizeData2, result+sizeResult);
        sizeResult = sizeResult+sizeData2;
    }


    if (data)
    {
        GLenum format = GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height*NrFrames, 0, format, GL_UNSIGNED_BYTE, result);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 7);

        glGenerateMipmap(GL_TEXTURE_2D);


        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load" << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int Model::TextureFromFile2(const char *path, const std::string &directory)
{
    glewInit();
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void Model::TextureFromFile3(const char *path, const std::string &directory, GLuint arrayTexture, int frameNumber)
{
    glewInit();
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, arrayTexture);

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
            1,                      //Mipmap number
            0, 0, frameNumber, //xoffset, yoffset, zoffset
            width, height, 1,          //width, height, depth
            GL_RGB,                 //format
            GL_UNSIGNED_BYTE,       //type
            data); //pointer to data

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
}
