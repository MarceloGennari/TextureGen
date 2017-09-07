#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>
#include "glew.h"
#include <GL/freeglut.h>

class Image{
    public:
        Image(std::string const &path, std::string const &ppmORpgm);
        Image(int width, int height, unsigned char *Arr, std::string grORrgb);
        Image(){}
        ~Image();

        void Display();
        void displayGray();        
        void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
        void setPixel(int x, int y, unsigned char gr);
        void setGray(unsigned char *gr){free(gray); gray = gr;}
        void setRGB(unsigned char *rgbArr){free(rgb); rgb = rgbArr;}
        void getPixel(int x, int y, unsigned char &r, unsigned char &g, unsigned char &b);
        void getPixel(int x, int y, unsigned char &gr);
        int getWidth(){return width;}
        int getHeight(){return height;}


    private:
        int width, height;
        unsigned char *rgb = NULL;
        unsigned char *gray = NULL;

        void loadPPM(std::string const &path);
        void loadPGM(std::string const &path);
        void getGrayScale();
        void static mainLoop();
};
#endif // IMAGE_H
