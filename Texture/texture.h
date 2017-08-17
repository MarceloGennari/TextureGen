#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "../stb_image.h"

class Texture
{
public:
    Texture(const std::string &n);
    void use();

private:
    unsigned int ID;
    std::string name;
    int width, height, nrChannels;
};

#endif // TEXTURE_H
