#include "image.h"
#include <iostream>
#include <fstream>
#include "pgmb_io.hpp"

Image::Image()
{

}

unsigned char * Image::loadPGM(std::string const &path){
//    std::string line;
//    std::ifstream file;
//    file.open(path.c_str());
//    if(file.is_open()){
//        while(getline(file, line)){
//            const char *c = line.c_str();
//            std::cout << (int)*c << std::endl;
//        }
//    }

//    file.close();
    int xsize, ysize;
    unsigned char *g;
    unsigned char maxg;

    bool err = pgmb_read(path, xsize, ysize, maxg, &g);
    if(!err){
        std::cout<<"working" << std::endl;
    }

}

unsigned wchar_t * Image::loadPPM(std::string const &path){

}

