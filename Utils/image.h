#ifndef IMAGE_H
#define IMAGE_H

#include <string>

class Image{
    public:
        Image();
        unsigned char * loadPGM(std::string const &path);
        unsigned wchar_t * loadPPM(std::string const &path);
        void Display();
    private:
        unsigned char* rgb;
        unsigned wchar_t * ppm;
};

#endif // IMAGE_H
