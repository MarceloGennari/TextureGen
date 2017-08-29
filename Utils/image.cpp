#include "image.h"
#include "shader.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>



Image::Image(std::string const &path, std::string const &ppmORpgm)
{
    if(ppmORpgm == "ppm"){
        loadPPM(path);

        // Makes sure that the class member gray is populated
        getGrayScale();
    }
}

Image::Image(int width, int height, unsigned char *Arr, std::string grORrgb){
    this->width = width;
    this->height = height;
    if(grORrgb == "gray"){
        this->gray = Arr;
        this->rgb = NULL;
    } else if(grORrgb == "rgb" ){
        this->rgb = Arr;
        getGrayScale();
    }
}


Image::~Image(){
//    if(rgb)
//        free(rgb);
//    if(gray)
//        free(gray);

//    rgb = NULL;
//    gray = NULL;
}

void Image::loadPPM(std::string const &path){
    // Note that we need to read binary PGM
    // The second argument specify that the stream will be interpreted as binary
    // The other ios::in specify that it will be used as input, not output
    /*
     * In the PPM format, a couple of information is crucial:
     * Since the width is given, the number of '\n' in the file is not the number of rows
     * Whenever a * appears, it means that that line has the same content as the one above it
     * */
    // I am assuming here that the Format will be P6


    std::fstream file(path.c_str(), std::ios::binary | std::ios::in);

    if(!file.is_open()){
        std::cout<< "Error" << std::endl;
        // Throw an exception
    } else {

        char c;
        /************** Header ************************/
        // This gets the P3 / P6 etc file format
        std::string format;
        file.get(c);
        format.push_back(c);
        file.get(c);
        format.push_back(c);
        file.get(c);

        if(format != "P6"){
            std::cout << "Image format is not P6 PPM" << std::endl;
            return;
        }

        /********** Width and Height *****************/
        // This gets the width and height of the file
        std::string wString, hString;

        // Getting Width
        do{
            file.get(c);
            wString.push_back(c);
        }while(c!=' ');
        std::istringstream convert(wString);
        convert>>width;

        // Getting Height
        do{
            file.get(c);
            hString.push_back(c);
        }while(c!='\n');
        std::istringstream converth(hString);
        converth>>height;
        file.get(c);

        /*************** Getting Size *****************/
        while(c!='\n'){
            file.get(c);
        }
        file.get(c);

        /************* BODY *******************/
        rgb = (unsigned char *)malloc(sizeof(unsigned char)*width*height*3);
        int counter = 0;
        do{
            rgb[counter] = c;
            counter++;
            file.get(c);

        }while(file.good());

        file.close();
    }
}
void Image::Display(){
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Image");
    glewInit();
    std::string vs = "/home/marcelo/TextureGen/Utils/GLSL/imvertsh.vsh";
    std::string fs = "/home/marcelo/TextureGen/Utils/GLSL/imfragsh.fsh";
    std::string geom = "/home/marcelo/TextureGen/Shader/GLSLSources/geometryshader.vert";
    Shader ourShader(vs.c_str(), geom.c_str(), fs.c_str());

    float vertices[] = {
            // positions          // colors           // texture coords
             1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // bottom right
             1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top right
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, // top left
            -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f  // bottom left
        };
    unsigned int indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO;
       glGenVertexArrays(1, &VAO);
       glGenBuffers(1, &VBO);
       glGenBuffers(1, &EBO);

       glBindVertexArray(VAO);

       glBindBuffer(GL_ARRAY_BUFFER, VBO);
       glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
       glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

       // position attribute
       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
       glEnableVertexAttribArray(0);
       // color attribute
       glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
       glEnableVertexAttribArray(1);
       // texture coord attribute
       glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
       glEnableVertexAttribArray(2);


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

       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb);
       glGenerateMipmap(GL_TEXTURE_2D);

       // render
       // ------
       glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
       glClear(GL_COLOR_BUFFER_BIT);

       // bind Texture
       glBindTexture(GL_TEXTURE_2D, texture);

       // render container
       ourShader.use();
       glBindVertexArray(VAO);
       glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

       // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
       // -------------------------------------------------------------------------------
       glutSwapBuffers();
}

void Image::getGrayScale(){
    int counter = 0;
    gray = (unsigned char *)malloc(sizeof(unsigned char)*width*height);
    for(int k = 0; k<width*height; k++){
        unsigned char red = 0.2126*rgb[counter++];
        unsigned char green = 0.7152*rgb[counter++];
        unsigned char blue = 0.0722*rgb[counter++];
        gray[k] = red+green+blue;
    }
}

void Image::displayGray(){
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Image Gray");
    glewInit();
    std::string vs = "/home/marcelo/TextureGen/Utils/GLSL/imvertsh.vsh";
    std::string fs = "/home/marcelo/TextureGen/Utils/GLSL/imfragsh.fsh";
    std::string geom = "/home/marcelo/TextureGen/Shader/GLSLSources/geometryshader.vert";
    Shader ourShader(vs.c_str(), geom.c_str(), fs.c_str());

    float vertices[] = {
            // positions          // colors           // texture coords
             1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // bottom right
             1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top right
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, // top left
            -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f  // bottom left
        };
    unsigned int indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO;
       glGenVertexArrays(1, &VAO);
       glGenBuffers(1, &VBO);
       glGenBuffers(1, &EBO);

       glBindVertexArray(VAO);

       glBindBuffer(GL_ARRAY_BUFFER, VBO);
       glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
       glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

       // position attribute
       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
       glEnableVertexAttribArray(0);
       // color attribute
       glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
       glEnableVertexAttribArray(1);
       // texture coord attribute
       glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
       glEnableVertexAttribArray(2);


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

       glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, gray);
       glGenerateMipmap(GL_TEXTURE_2D);

       // render
       // ------
       glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
       glClear(GL_COLOR_BUFFER_BIT);

       // bind Texture
       glBindTexture(GL_TEXTURE_2D, texture);

       // render container
       ourShader.use();
       glBindVertexArray(VAO);
       glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

       // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
       // -------------------------------------------------------------------------------
       glutSwapBuffers();
}


/**************
 *  Methods for RGB Image Manipulation
 * ************/

void Image::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b){    
    /* The idea here is to convert from the matrix format to the stream of data format
     * */
    int index = 0;

    index = (y-1)*3*width;
    index = index + (x-1)*3;
    rgb[index++] = r;
    rgb[index++] = g;
    rgb[index] = b;
}

void Image::getPixel(int x, int y, unsigned char &r, unsigned char &g, unsigned char &b){
    /* The idea is to convert from the matrix format to the stream format
     * */
    int index = 0;
    index = (y-1)*3*width;
    index = index + (x-1)*3;
    r = rgb[index++];
    g = rgb[index++];
    b = rgb[index];
}

/**************
 *  Overloaded methods for Grayscale Image Manipulation
 * ************/

void Image::setPixel(int x, int y, unsigned char gr){
    /* The idea is again convert the matrix format to the stream of data format
     * However, since it is using grayscale, it is a bit different:
     * */
    int index = 0;
    index = (y-1)*width;
    index = index + (x-1);
    gray[index] = gr;
}

void Image::getPixel(int x, int y, unsigned char &gr){
    /* The idea is again convert the matrix format to the stream of data format
     * However, since it is using grayscale, it is a bit different:
     * */
    int index = 0;
    index = (y-1)*width;
    index = index + (x-1);
    gr = gray[index];
}

























