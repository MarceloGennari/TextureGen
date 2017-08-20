#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "glm/glm.hpp"

class Shader
{
public:
    Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);
    void use();

    // Setter for Uniforms
    void setVec4(const std::string &name, glm::vec4 &vec);
    void setVec3(const std::string &name, glm::vec3 &vec);
    void setVec2(const std::string &name, glm::vec2 &vec);
    void setFloat(const std::string &name, float un);
    void setInt(const std::string &name, float in);
    void setMat4(const std::string &name, glm::mat4 &vec);
    void setBool(const std::string& name, bool b);
private:
    unsigned int programID;
};

#endif // SHADER_H
