#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader() : ID(0) {} 

Shader::Shader(const char* vertexPath, const char* fragmentPath) : ID(0) {
    loadFromFile(vertexPath, fragmentPath);
}

bool Shader::loadFromFile(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    if (vertexCode.empty() || fragmentCode.empty()) {
        return false;
    }

    GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    if (vertex == 0 || fragment == 0) {
        if (vertex != 0) glDeleteShader(vertex);
        if (fragment != 0) glDeleteShader(fragment);
        return false;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    if (!checkCompileErrors(ID, "PROGRAM")) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(ID);
        ID = 0;
        return false;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return true;
}

std::string Shader::readFile(const char* filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "ERROR::SHADER::FILE_NOT_FOUND: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

GLuint Shader::compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    if (!checkCompileErrors(shader, (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT"))) {
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
                << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog
                << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    return success;
}

void Shader::use() const {
    if (ID != 0) {
        glUseProgram(ID);
    }
    else {
        // std::cerr << "Warning: Trying to use shader with ID 0." << std::endl;
    }
}

void Shader::setBool(const std::string& name, bool value) const { if (ID != 0) glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); }
void Shader::setInt(const std::string& name, int value) const { if (ID != 0) glUniform1i(glGetUniformLocation(ID, name.c_str()), value); }
void Shader::setFloat(const std::string& name, float value) const { if (ID != 0) glUniform1f(glGetUniformLocation(ID, name.c_str()), value); }
void Shader::setVec2(const std::string& name, const glm::vec2& value) const { if (ID != 0) glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value)); }
void Shader::setVec3(const std::string& name, const glm::vec3& value) const { if (ID != 0) glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value)); }
void Shader::setVec4(const std::string& name, const glm::vec4& value) const { if (ID != 0) glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value)); }
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const { if (ID != 0) glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat)); }
