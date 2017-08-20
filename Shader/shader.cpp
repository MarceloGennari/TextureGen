#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "GL/glew.h"
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertexPath, const char *geometryPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
       std::string vertexCode;
       std::string geometryCode;
       std::string fragmentCode;
       std::ifstream vShaderFile;
       std::ifstream gShaderFile;
       std::ifstream fShaderFile;
       // ensure ifstream objects can throw exceptions:
       vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
       gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
       fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
       try
       {
           // open files
           vShaderFile.open(vertexPath);
           gShaderFile.open(geometryPath);
           fShaderFile.open(fragmentPath);
           std::stringstream vShaderStream, gShaderStream, fShaderStream;
           // read file's buffer contents into streams
           vShaderStream << vShaderFile.rdbuf();
           gShaderStream << gShaderFile.rdbuf();
           fShaderStream << fShaderFile.rdbuf();
           // close file handlers
           vShaderFile.close();
           gShaderFile.close();
           fShaderFile.close();
           // convert stream into string
           vertexCode   = vShaderStream.str();
           geometryCode = gShaderStream.str();
           fragmentCode = fShaderStream.str();
       }
       catch(std::ifstream::failure e)
       {
           std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            std::cout<< e.what() << std::endl;
       }
       const char* vShaderCode = vertexCode.c_str();
       const char* gShaderCode = geometryCode.c_str();
       const char* fShaderCode = fragmentCode.c_str();

       // 2. compile shaders
       unsigned int vertex, geometry, fragment;
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

       // similar for geometry shader
       geometry = glCreateShader(GL_GEOMETRY_SHADER);
       glShaderSource(geometry, 1, &gShaderCode, NULL);
       glCompileShader(geometry);
       // print compile errors if any
       glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
       if(!success){
           glGetShaderInfoLog(geometry, 512, NULL, infoLog);
           std::cout<<"ERROR::SAHDER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
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
       glAttachShader(programID, geometry);
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
       glDeleteShader(geometry);
       glDeleteShader(fragment);
}

void Shader::use(){
    glUseProgram(programID);
}

void Shader::setVec4(const std::string &name, glm::vec4 &vec){
    GLfloat x = vec.x; GLfloat y = vec.y; GLfloat z = vec.z; GLfloat w = vec.w;
    glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
}

void Shader::setVec3(const std::string &name, glm::vec3 &vec){
    GLfloat x = vec.x; GLfloat y = vec.y; GLfloat z = vec.z;
    glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}

void Shader::setVec2(const std::string &name, glm::vec2 &vec){
    GLfloat x = vec.x; GLfloat y = vec.y;
    glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
}

void Shader::setFloat(const std::string &name, float un){
    GLfloat Gun = un;
    glUniform1f(glGetUniformLocation(programID, name.c_str()), Gun);
}

void Shader::setInt(const std::string &name, float in){
    GLint Gin = in;
    glUniform1i(glGetUniformLocation(programID, name.c_str()), Gin);
}

void Shader::setMat4(const std::string &name, glm::mat4 &vec){
    glUniformMatrix4fv(glGetUniformLocation(programID,name.c_str()),1, GL_FALSE, glm::value_ptr(vec));
}

void Shader::setBool(const std::string &name, bool in){
    GLint Gin = in;
    glUniform1i(glGetUniformLocation(programID, name.c_str()), Gin);
}
