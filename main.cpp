#include <cstdlib>
#include <iostream>
#include "GL/freeglut.h"
#include "shader.h";

using namespace std;

void render(){
    glClearColor(1.0,1.0, 1.0, 1.0);
    std::string fragm = "fragmentShader.fsh";
    std::string vec = "vectorShader.fsh";
    Shader sh(fragm, vec);

    glutSwapBuffers();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Test");
    glutDisplayFunc(render);
    glutIdleFunc(render);
    glutMainLoop();
    return 0;
}
