#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 

class Shader
{
public:
    GLuint ID;

    Shader(); 

    Shader(const char* vertexPath, const char* fragmentPath);

    bool loadFromFile(const char* vertexPath, const char* fragmentPath);

    void use() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    std::string readFile(const char* filePath);
    GLuint compileShader(GLenum type, const std::string& source);
    bool checkCompileErrors(GLuint shader, std::string type);
};
