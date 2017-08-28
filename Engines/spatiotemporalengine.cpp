#include "textureengine.h"
#include "CImg.h"
#include "stb_image.h"

using namespace cimg_library;

void TextureEngine::SaptiotemporalEngine::testingSTB(){
    int width, height, dpp;
    unsigned char * rgb = stbi_load("/home/marcelo/TextureGen/Teddy/Frames/0000.ppm", &width, &height, &dpp, 0);

}
