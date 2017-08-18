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
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

void render(){

    glClearColor(1.0,1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glewInit();

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // point 0
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // point 1
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // point 2

         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // point 2
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // point 3
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // point 0

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // point 4
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // point 5
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // point 6

        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // point 6
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // point 7
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // point 4

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,

        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
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


    std::string fragm = "/home/marcelo/Desktop/TextureGen/Shader/GLSLSources/fragmentshader.fsh";
    std::string vec = "/home/marcelo/Desktop/TextureGen/Shader/GLSLSources/vertexshader.fsh";
    Shader sh(vec.c_str(), fragm.c_str());

    /**********
     * Camera
     * ********/
    glm::mat4 model;
    model = glm::rotate(model,glm::radians(-55.0f), glm::vec3(1.0f,0.0f,0.0f));

    glm::mat4 view = Camera::getCam()->getView();
    glm::mat4 projection = Camera::getCam()->getProjection();

    Model test("/home/marcelo/Downloads/darksouls/EliteKnight.stl");


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
    sh.use();
    sh.setMat4("view", view);
    sh.setMat4("model", model);
    sh.setMat4("projection", projection);

   // sh.setInt("brick", 0);

//    glDrawArrays(GL_TRIANGLES, 0, 36);

//    m.Draw(sh);
    test.Draw(sh);

    glutSwapBuffers();
}
void keyboardInput(unsigned char key, int x, int y){
    Camera *cam = Camera::getCam();

    switch(key){
        case 'w':
            cam->setCamPos(cam->getCamPos() + glm::vec3(0.0f, 0.0f, -0.1f));
            cam->setTargetPos(cam->getTargetPos()+glm::vec3(0.0f, 0.0f, -0.1f));
            break;
        case 'a':
            cam->setCamPos(cam->getCamPos() + glm::vec3(-0.1f, 0.0f,0.0f));
            cam->setTargetPos(cam->getTargetPos()+glm::vec3(-0.1f, 0.0f, 0.0f));

            break;
        case 's':
            cam->setCamPos(cam->getCamPos() + glm::vec3(0.0f, 0.0f, 0.1f));
            cam->setTargetPos(cam->getTargetPos()+glm::vec3(0.0f, 0.0f, 0.1f));
            break;
        case 'd':
            cam->setCamPos(cam->getCamPos() + glm::vec3(0.1f, 0.0f,0.0f));
            cam->setTargetPos(cam->getTargetPos()+glm::vec3(0.1f, 0.0f, 0.0f));
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glewInit();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Test");

    glm::mat4 view;
    Camera::getCam()->setProjection(glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.f));
    Camera::getCam()->setCamPos(glm::vec3(1.0f, 1.0f, 3.0f));
    Camera::getCam()->setUpPos(glm::vec3(0.0f, 1.0f, 0.0f));
    Camera::getCam()->setTargetPos(glm::vec3(0.0f, 0.0f, 0.0f));

    Camera::getCam()->setView(glm::lookAt(Camera::getCam()->getCamPos(),
                                          Camera::getCam()->getTargetPos(),
                                          Camera::getCam()->getUpPos()));
    glutKeyboardFunc(keyboardInput);
    glutIdleFunc(render);
    glutMainLoop();
    return 0;
}
