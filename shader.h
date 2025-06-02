#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // Dla glm::value_ptr

class Shader
{
public:
    GLuint ID;

    Shader(); // Domyœlny konstruktor

    // Konstruktor, który od razu ³aduje shadery
    Shader(const char* vertexPath, const char* fragmentPath);

    // Metoda do za³adowania shaderów (jeœli u¿yto domyœlnego konstruktora)
    bool loadFromFile(const char* vertexPath, const char* fragmentPath);

    void use() const;

    // Metody do ustawiania uniformów
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