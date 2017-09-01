#include <GL/glew.h>
#include <cstdlib>
#include <iostream>

#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "model.h"
#include "light.h"
#include "image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void render(){

    glClearColor(0.9,0.9, 0.9, 1.0);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float points[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // point 0
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // point 1
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // point 2

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // point 3

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // point 4
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // point 5
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // point 6

        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // point 7

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // point 8
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    std::vector<TextureS> Textured;
    TextureS t;
    t.id = 0;
    t.type = "texture_diffuse";
    Textured.push_back(t);

    Vertex v;
    int index;
    for(int i = 0; i<24; i++){
        index = i*5;
        v.Pos.x = points[index++];
        v.Pos.y = points[index++];
        v.Pos.z = points[index++];
        v.TexCoords.x = points[index++];
        v.TexCoords.y = points[index++];
        Vertices.push_back(v);
    }
    for(int i =0; i<36; i++){
        Indices.push_back(indices[i]);
    }

    Mesh m(Vertices, Indices, Textured);


    std::string fragm = "/home/marcelo/TextureGen/Shader/GLSLSources/fragmentshader.fsh";
    std::string geom = "/home/marcelo/TextureGen/Shader/GLSLSources/geometryshader.vert";
    std::string vec = "/home/marcelo/TextureGen/Shader/GLSLSources/vertexshader.fsh";
    Shader sh(vec.c_str(), geom.c_str(), fragm.c_str());

    /**********
     * Camera
     * ********/
    glm::mat4 model;
    //model = glm::rotate(model,glm::radians(180.0f), glm::vec3(0.0f,1.0f,0.0f));

    glm::mat4 view = Camera::getCam()->getView();
    glm::mat4 projection = Camera::getCam()->getProjection();

    /***********
     * Texture
     * *********/
    // load and create a texture
    // -------------------------
    //Texture boxTex("/home/marcelo/Desktop/TextureGen/container.jpg");

    /**********
     * Object
     * ********/
//    unsigned int VBO, VAO;
//    glGenVertexArrays(1, &VAO);
//    glBindVertexArray(VAO);

//    glGenBuffers(1, &VBO);
//    glBindBuffer(GL_ARRAY_BUFFER,VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices, GL_STATIC_DRAW);

//    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);

//    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 5*sizeof(float),(void*)(3*sizeof(float)));
//    glEnableVertexAttribArray(1);

//    boxTex.use();
    Light light(glm::vec3(500.0f, 500.0f, 500.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    glm::vec3 Colour = light.getColor();
    glm::vec3 lightPos = light.getPos();
    glm::vec3 Obj = glm::vec3(0.7f, 0.7f, 0.7f);
    sh.use();
    sh.setMat4("view", view);
    sh.setMat4("model", model);
    sh.setMat4("projection", projection);
    sh.setVec3("lightColour", Colour);
    sh.setVec3("objectColour", Obj);
    sh.setVec3("lightPos", lightPos);

   // sh.setInt("brick", 0);

//    glDrawArrays(GL_TRIANGLES, 0, 36);

    //m.Draw(sh);
    Model::getModel()->Draw(sh);

    glutSwapBuffers();
}

int main(int argc, char *argv[])
{


    glutInit(&argc, argv);
    glewExperimental = GL_TRUE;
    glewInit();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Test");
    glutKeyboardFunc(Camera::keyBoardInput);
    glutMouseFunc(Camera::mouseInput);
    glutIdleFunc(render);

    Camera::getCam()->positionCameraFrN("349");

    //Model::getModel()->loadModel("/home/marcelo/Downloads/nanosuit/nanosuit.obj");
    //Model::getModel()->loadModel("/home/marcelo/Downloads/Species/files/maui_dolphin.stl");
    //Model::getModel()->loadModel("/home/marcelo/Downloads/EliteKnight/EliteKnight.stl");
    Model::getModel()->loadModel("/home/marcelo/TextureGen/Teddy/mesh.stl");



    glutMainLoop();
    return 0;
}
