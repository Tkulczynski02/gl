#pragma once 

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shader.h" 

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords; 
};

struct Texture {
    unsigned int id;
    std::string type; 
    std::string path; 
};

class Mesh {
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures; 
    unsigned int VAO;

    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);

    void Draw(Shader& shader);

private:
    unsigned int VBO, EBO;
    void setupMesh();
};
