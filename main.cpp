#include <GL/glew.h>
#include <cstdlib>
#include <iostream>

#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"
#include "camera.h"

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

    glm::vec3 colour(1.0,0.0,0.0);

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
       unsigned int texture;
       glGenTextures(1, &texture);
       glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
       // set the texture wrapping parameters
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
       // set texture filtering parameters
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       // load image, create texture and generate mipmaps
       int width, height, nrChannels;
       // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
       unsigned char *data = stbi_load("/home/marcelo/TextureGen/container.jpg", &width, &height, &nrChannels, 0);
       if (data)
       {
           glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
           glGenerateMipmap(GL_TEXTURE_2D);
       }
       else
       {
           std::cout << "Failed to load texture" << std::endl;
       }
       stbi_image_free(data);

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


    glBindTexture(GL_TEXTURE_2D, texture);

    sh.use();
    sh.setVec3("vertexColor", colour);
    sh.setMat4("view", view);
    sh.setMat4("model", model);
    sh.setMat4("projection", projection);
    sh.setInt("brick", 0);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glutSwapBuffers();
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

    glutDisplayFunc(render);
    glutIdleFunc(render);
    glutMainLoop();
    return 0;
}
