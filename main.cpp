#include <cstdlib>
#include <iostream>
#include <GL/glew.h>
#include "GL/freeglut.h"

#include "shader.h"
#include "glm/glm.hpp"

using namespace std;

void render(){

    glClearColor(1.0,1.0, 1.0, 1.0);
    glewInit();

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    std::string fragm = "/home/marcelo/Desktop/TextureGen/Shader/GLSLSources/fragmentshader.fsh";
    std::string vec = "/home/marcelo/Desktop/TextureGen/Shader/GLSLSources/vertexshader.fsh";
    Shader sh(fragm.c_str(), vec.c_str());

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    sh.use();
    glDrawArrays(GL_TRIANGLES, 0,3);
    glutSwapBuffers();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glewInit();
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE  );
    glutInitWindowSize(600, 600);
    glutCreateWindow("Test");
    glutDisplayFunc(render);
    glutIdleFunc(render);
    glutMainLoop();
    return 0;
}
