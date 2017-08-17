#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "glm/glm.hpp"

class Shader
{
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    void use();

    // Setter for Uniforms
    void setVec4(std::string &name, glm::vec4 &vec);
    void setVec3(std::string &name, glm::vec3 &vec);
    void setVec2(std::string &name, glm::vec2 &vec);
    void setFloat(std::string &name, float un);
    void setInt(std::string &name, float in);

private:
    unsigned int programID;
};

#endif // SHADER_H
