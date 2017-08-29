#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>
#include "glew.h"
#include <GL/freeglut.h>

class Image{
    public:
        Image();
        std::vector<unsigned char> loadPGM(std::string const &path);
        unsigned wchar_t * loadPPM(std::string const &path);
        void Display();
    private:
        unsigned char * rgb;
        unsigned wchar_t * ppm;
        int width, height;
};

#endif // IMAGE_H
