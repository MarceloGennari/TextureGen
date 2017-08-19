#include "model.h"
#include <iostream>
#include <algorithm>

Model* Model::m;

Model::Model(){}

void Model::Draw(Shader shader){


    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);



    //PairSelection();
}

void Model::unRepVert(std::vector<Vertex> &vs, std::vector<unsigned int> &ind){
    /*
     * The idea of this algorithm is:
     * we iterate from 0 to the size of the vector and compare each other
     * if we find two that are equal, we substitute that one in the EBO
     * the "continue" part is in case we try to check the same vertix again.
     * the i-count is because the vertex 300 in the old scheme is going to become 300-count where count is the number of vertices that were taken out
   */
    std::vector <unsigned int> newInd;
    std::vector <Vertex> newVs;
    std::vector <unsigned int> repeatedVs;

    newInd = ind;
    for(unsigned int i = 0; i<vs.size(); i++){
        if(std::find(repeatedVs.begin(), repeatedVs.end(), i) != repeatedVs.end() ){
            continue;
        }
        newVs.push_back(vs[i]);
        newInd[i] = newVs.size()-1;

        for(unsigned int j=i+1; j<vs.size(); j++){
            // Apparently there are vertices with the same position but different normal values
            // Since the code wants vertex at same position (and since we can calculate normals in the geometry shader)
            // I am going to stick with the "same vertex position" approach
                if(vs[i].Pos == vs[j].Pos /*&& vs[i].Normal == vs[j].Normal*/){
                    repeatedVs.push_back(j);
                    newInd[j] = newVs.size()-1;
                }
        }
    }

    vs = newVs;
    ind = newInd;

}
void Model::PairSelection(){


}

void Model::loadModel(std::string path){
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

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
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        // tangent
        if(mesh->mTangents != NULL){
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
        }
        // bitangent
        if(mesh->mBitangents !=NULL){
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
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
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    std::vector<TextureS> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<TextureS> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<TextureS> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<TextureS> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data

    /* Notice that when the File is .stl, there will be loads of repeated vertices.
     * This is because stl doesn't use EBOs
     * So the next function tries to rectify this by assigning the right indices to the right vertices.
     * **/
    unRepVert(vertices, indices);

    return Mesh(vertices, indices, textures);
}

std::vector<TextureS> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
   {
       std::vector<TextureS> textures;
       for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
       {
           aiString str;
           mat->GetTexture(type, i, &str);
           // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
           bool skip = false;
           for(unsigned int j = 0; j < textures_loaded.size(); j++)
           {
               if(std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
               {
                   textures.push_back(textures_loaded[j]);
                   skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                   break;
               }
           }
           if(!skip)
           {   // if texture hasn't been loaded already, load it
               TextureS texture;
               texture.id = TextureFromFile(str.C_Str(), this->directory);
               texture.type = typeName;
               texture.path = str;
               textures.push_back(texture);
               textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
           }
       }
       return textures;
   }


unsigned int Model::TextureFromFile(const char *path, const std::string &directory)
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
