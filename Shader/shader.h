#ifndef SHADER_H
#define SHADER_H

#include <string>

class Shader
{
public:
    Shader(std::string pathToVertex,std::string pathToFragment);
    void compile();
    void link();

private:
    unsigned int programID;
};

#endif // SHADER_H
