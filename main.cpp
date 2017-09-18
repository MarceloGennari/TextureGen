#include <GL/glew.h>
#include <cstdlib>
#include <iostream>

#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "mesh.h"
#include "shader.h"
#include "camera.h"
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

    std::string fragm = "/home/marcelo/TextureGen/Shader/GLSLSources/fragmentshader.fsh";
    std::string vec = "/home/marcelo/TextureGen/Shader/GLSLSources/vertexshader.fsh";
    Shader sh(vec.c_str(), fragm.c_str());

    /**********
     * Camera
     * ********/
    glm::mat4 model;

    glm::mat4 view = Camera::getCam()->getView();
    glm::mat4 projection = Camera::getCam()->getProjection();

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
    glutCreateWindow("InfiniTAM with Texture");
    glutKeyboardFunc(Camera::keyBoardInput);
    glutMouseFunc(Camera::mouseInput);
    glutIdleFunc(render);

    Model::getModel()->loadModel("/home/marcelo/TextureGen/Teddy/mesh.stl");

    glutMainLoop();
    return 0;
}
