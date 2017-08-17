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
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

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

    std::string fragm = "/home/marcelo/TextureGen/Shader/GLSLSources/fragmentshader.fsh";
    std::string vec = "/home/marcelo/TextureGen/Shader/GLSLSources/vertexshader.fsh";
    Shader sh(vec.c_str(), fragm.c_str());

    /**********
     * Camera
     * ********/
    glm::mat4 model;
    model = glm::rotate(model,glm::radians(-55.0f), glm::vec3(1.0f,0.0f,0.0f));

    glm::mat4 view = Camera::getCam()->getView();
    glm::mat4 projection = Camera::getCam()->getProjection();

    /***********
     * Texture
     * *********/
    // load and create a texture
    // -------------------------
    Texture boxTex("/home/marcelo/TextureGen/container.jpg");

    /**********
     * Object
     * ********/
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 5*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    boxTex.use();
    sh.use();
    sh.setMat4("view", view);
    sh.setMat4("model", model);
    sh.setMat4("projection", projection);
    sh.setInt("brick", 0);

    glDrawArrays(GL_TRIANGLES, 0, 36);

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
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE  );
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
