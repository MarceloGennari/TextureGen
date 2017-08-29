#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>
#include "glew.h"
#include <GL/freeglut.h>

class Image{
    public:
        Image(std::string const &path, std::string const &ppm);
        ~Image();

        unsigned wchar_t * loadPGM(std::string const &path);
        void Display();
        void displayGray();
        void getGrayScale();
        void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);

    private:
        int width, height;
        unsigned char *rgb;
        unsigned char *gray;

        void loadPPM(std::string const &path);

};

#endif // IMAGE_H
