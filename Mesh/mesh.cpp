#include "mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->setUpMesh();
}

void Mesh::Draw(Shader shader){
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::stringstream ss;
        std::string number;
        std::string name = textures[i].type;
        if(name == "texture_diffuse")
            ss << diffuseNr++; // transfer unsigned int to stream
        else if(name == "texture_specular")
            ss << specularNr++; // transfer unsigned int to stream
        number = ss.str();

        shader.setFloat(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    shader.use();

    if (textures.size()==0)
        shader.setBool("isTex", 0);
    else
        shader.setBool("isTex", 1);
    // draw mesh
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::setUpMesh(){
       glewInit();
       unsigned int A, B, E;
       glGenVertexArrays(1, &A);
       glGenBuffers(1, &B);
       glGenBuffers(1, &E);

       this->VAO = A;
       this->VBO = B;
       this->EBO = E;

       glBindVertexArray(VAO);
       glBindBuffer(GL_ARRAY_BUFFER, VBO);

       glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
       glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                    &indices[0], GL_STATIC_DRAW);

       // vertex positions
       glEnableVertexAttribArray(0);
       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
       // vertex normals
       glEnableVertexAttribArray(1);
       glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
       // vertex texture coords
       glEnableVertexAttribArray(2);
       glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

       glBindVertexArray(0);
}
