#include "Platform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector> 

// Funkcja pomocnicza do dodawania wierzchołków prostopadłościanu o zadanych wymiarach,
// wycentrowanego wokół lokalnego (0,0,0)
void addCuboidVertices(std::vector<float>& vertices, float width, float height, float depth) {
    float hx = width / 2.0f;
    float hy = height / 2.0f;
    float hz = depth / 2.0f;

    // Pozycje względem lokalnego środka (0,0,0)
    // Przód (+Z) nx=0, ny=0, nz=1
    vertices.insert(vertices.end(), { -hx,-hy, hz, 0,0,1,  hx,-hy, hz, 0,0,1,  hx, hy, hz, 0,0,1,  hx, hy, hz, 0,0,1, -hx, hy, hz, 0,0,1, -hx,-hy, hz, 0,0,1 });
    // Tył (-Z) nx=0, ny=0, nz=-1
    vertices.insert(vertices.end(), { -hx,-hy,-hz, 0,0,-1,  hx,-hy,-hz, 0,0,-1,  hx, hy,-hz, 0,0,-1,  hx, hy,-hz, 0,0,-1, -hx, hy,-hz, 0,0,-1, -hx,-hy,-hz, 0,0,-1 });
    // Lewo (-X) nx=-1, ny=0, nz=0
    vertices.insert(vertices.end(), { -hx,-hy,-hz, -1,0,0, -hx,-hy, hz, -1,0,0, -hx, hy, hz, -1,0,0, -hx, hy, hz, -1,0,0, -hx, hy,-hz, -1,0,0, -hx,-hy,-hz, -1,0,0 });
    // Prawo (+X) nx=1, ny=0, nz=0
    vertices.insert(vertices.end(), { hx,-hy,-hz,  1,0,0,  hx,-hy, hz,  1,0,0,  hx, hy, hz,  1,0,0,  hx, hy, hz,  1,0,0,  hx, hy,-hz,  1,0,0,  hx,-hy,-hz,  1,0,0 });
    // Dół (-Y) nx=0, ny=-1, nz=0
    vertices.insert(vertices.end(), { -hx,-hy,-hz, 0,-1,0,  hx,-hy,-hz, 0,-1,0,  hx,-hy, hz, 0,-1,0,  hx,-hy, hz, 0,-1,0, -hx,-hy, hz, 0,-1,0, -hx,-hy,-hz, 0,-1,0 });
    // Góra (+Y) nx=0, ny=1, nz=0
    vertices.insert(vertices.end(), { -hx, hy,-hz, 0,1,0,  hx, hy,-hz, 0,1,0,  hx, hy, hz, 0,1,0,  hx, hy, hz, 0,1,0, -hx, hy, hz, 0,1,0, -hx, hy,-hz, 0,1,0 });
}


Platform::Platform(glm::vec3 center_position, glm::vec3 dimensions)
    : position(center_position), size(dimensions), VAO(0), VBO(0), vertexCount(0)
{
    setupPlatformModel();
    // std::cout << "[Platform] Created at (" << position.x << "," << position.y << "," << position.z << ")"
    //     << " with size (" << size.x << "," << size.y << "," << size.z << ")" << std::endl;
}

Platform::~Platform() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // std::cout << "[Platform] Destroyed." << std::endl;
}

void Platform::setupPlatformModel() {
    std::vector<float> platformVertexData;

    addCuboidVertices(platformVertexData, this->size.x, this->size.y, this->size.z);

    vertexCount = platformVertexData.size() / 6; 

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, platformVertexData.size() * sizeof(float), platformVertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // std::cout << "[Platform] Model setup. Vertices: " << vertexCount << std::endl;
}


void Platform::Draw(Shader& shader) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, this->position);

    // Usunięto rotację, aby uprościć kolizje AABB.
    // Jeśli chcesz obrócone budynki, musisz zaimplementować kolizje OBB.
    // float rotationAngleDegrees = 90.0f; 
    // glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngleDegrees), rotationAxis);

    shader.setMat4("model", modelMatrix);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount));
    glBindVertexArray(0);
}
