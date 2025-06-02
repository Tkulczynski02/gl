#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>

class Shader;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class modelloader {
public:
    modelloader(const std::string& path);
    void Draw(Shader& shader);

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;

    void loadModel(const std::string& path);
    void processMesh(aiMesh* mesh, const aiScene* scene);
    void setupMesh();
};
