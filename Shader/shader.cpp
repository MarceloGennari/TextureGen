#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "GL/glew.h"
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
       std::string vertexCode;
       std::string fragmentCode;
       std::ifstream vShaderFile;
       std::ifstream fShaderFile;
       // ensure ifstream objects can throw exceptions:
       vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
       fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
       try
       {
           // open files
           vShaderFile.open(vertexPath);
           fShaderFile.open(fragmentPath);
           std::stringstream vShaderStream, fShaderStream;
           // read file's buffer contents into streams
           vShaderStream << vShaderFile.rdbuf();
           fShaderStream << fShaderFile.rdbuf();
           // close file handlers
           vShaderFile.close();
           fShaderFile.close();
           // convert stream into string
           vertexCode   = vShaderStream.str();
           fragmentCode = fShaderStream.str();
       }
       catch(std::ifstream::failure e)
       {
           std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            std::cout<< e.what() << std::endl;
       }
       const char* vShaderCode = vertexCode.c_str();
       const char* fShaderCode = fragmentCode.c_str();

       // 2. compile shaders
       unsigned int vertex, fragment;
       int success;
       char infoLog[512];

       // vertex Shader
       vertex = glCreateShader(GL_VERTEX_SHADER);
       glShaderSource(vertex, 1, &vShaderCode, NULL);
       glCompileShader(vertex);
       // print compile errors if any
       glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
       if(!success)
       {
           glGetShaderInfoLog(vertex, 512, NULL, infoLog);
           std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
       };

       // similiar for Fragment Shader
       fragment = glCreateShader(GL_FRAGMENT_SHADER);
       glShaderSource(fragment, 1, &fShaderCode, NULL);
       glCompileShader(fragment);
       // print compile errors if any
       glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
       if(!success){
           glGetShaderInfoLog(fragment, 512, NULL, infoLog);
           std::cout<<"ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
       };

       // shader Program
       programID = glCreateProgram();
       glAttachShader(programID, vertex);
       glAttachShader(programID, fragment);
       glLinkProgram(programID);
       // print linking errors if any
       glGetProgramiv(programID, GL_LINK_STATUS, &success);
       if(!success)
       {
           glGetProgramInfoLog(this->programID, 512, NULL, infoLog);
           std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
       }

       // delete the shaders as they're linked into our program now and no longer necessery
       glDeleteShader(vertex);
       glDeleteShader(fragment);
}

void Shader::use(){
    glUseProgram(programID);
}

void Shader::setVec4(std::string &name, glm::vec4 &vec){
    GLfloat x = vec.x; GLfloat y = vec.y; GLfloat z = vec.z; GLfloat w = vec.w;
    glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
}

void Shader::setVec3(std::string &name, glm::vec3 &vec){
    GLfloat x = vec.x; GLfloat y = vec.y; GLfloat z = vec.z;
    glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}

void Shader::setVec2(std::string &name, glm::vec2 &vec){
    GLfloat x = vec.x; GLfloat y = vec.y;
    glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
}

void Shader::setFloat(std::string &name, float un){
    GLfloat Gun = un;
    glUniform1f(glGetUniformLocation(programID, name.c_str()), Gun);
}

void Shader::setInt(std::string &name, float in){
    GLfloat Gin = in;
    glUniform1i(glGetUniformLocation(programID, name.c_str()), Gin);
}
