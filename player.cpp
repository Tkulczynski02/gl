#include "Player.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <GL/glew.h> 

// Funkcja addCubeVertices (bez zmian - z poprzednich odpowiedzi)
void addCubeVertices(std::vector<float>& vertices, glm::vec3 center, float dx, float dy, float dz) {
    float hx = dx / 2.0f; float hy = dy / 2.0f; float hz = dz / 2.0f;
    // Front face (+Z)
    vertices.insert(vertices.end(), { center.x - hx,center.y - hy,center.z + hz, 0.0f,0.0f,1.0f, center.x + hx,center.y - hy,center.z + hz, 0.0f,0.0f,1.0f, center.x + hx,center.y + hy,center.z + hz, 0.0f,0.0f,1.0f, center.x + hx,center.y + hy,center.z + hz, 0.0f,0.0f,1.0f, center.x - hx,center.y + hy,center.z + hz, 0.0f,0.0f,1.0f, center.x - hx,center.y - hy,center.z + hz, 0.0f,0.0f,1.0f });
    // Back face (-Z)
    vertices.insert(vertices.end(), { center.x - hx,center.y - hy,center.z - hz, 0.0f,0.0f,-1.0f, center.x + hx,center.y + hy,center.z - hz, 0.0f,0.0f,-1.0f, center.x + hx,center.y - hy,center.z - hz, 0.0f,0.0f,-1.0f, center.x + hx,center.y + hy,center.z - hz, 0.0f,0.0f,-1.0f, center.x - hx,center.y - hy,center.z - hz, 0.0f,0.0f,-1.0f, center.x - hx,center.y + hy,center.z - hz, 0.0f,0.0f,-1.0f });
    // Left face (-X)
    vertices.insert(vertices.end(), { center.x - hx,center.y + hy,center.z + hz, -1.0f,0.0f,0.0f, center.x - hx,center.y + hy,center.z - hz, -1.0f,0.0f,0.0f, center.x - hx,center.y - hy,center.z - hz, -1.0f,0.0f,0.0f, center.x - hx,center.y - hy,center.z - hz, -1.0f,0.0f,0.0f, center.x - hx,center.y - hy,center.z + hz, -1.0f,0.0f,0.0f, center.x - hx,center.y + hy,center.z + hz, -1.0f,0.0f,0.0f });
    // Right face (+X)
    vertices.insert(vertices.end(), { center.x + hx,center.y + hy,center.z + hz, 1.0f,0.0f,0.0f, center.x + hx,center.y - hy,center.z - hz, 1.0f,0.0f,0.0f, center.x + hx,center.y + hy,center.z - hz, 1.0f,0.0f,0.0f, center.x + hx,center.y - hy,center.z - hz, 1.0f,0.0f,0.0f, center.x + hx,center.y + hy,center.z + hz, 1.0f,0.0f,0.0f, center.x + hx,center.y - hy,center.z + hz, 1.0f,0.0f,0.0f });
    // Bottom face (-Y)
    vertices.insert(vertices.end(), { center.x - hx,center.y - hy,center.z - hz, 0.0f,-1.0f,0.0f, center.x + hx,center.y - hy,center.z - hz, 0.0f,-1.0f,0.0f, center.x + hx,center.y - hy,center.z + hz, 0.0f,-1.0f,0.0f, center.x + hx,center.y - hy,center.z + hz, 0.0f,-1.0f,0.0f, center.x - hx,center.y - hy,center.z + hz, 0.0f,-1.0f,0.0f, center.x - hx,center.y - hy,center.z - hz, 0.0f,-1.0f,0.0f });
    // Top face (+Y)
    vertices.insert(vertices.end(), { center.x - hx,center.y + hy,center.z - hz, 0.0f,1.0f,0.0f, center.x + hx,center.y + hy,center.z + hz, 0.0f,1.0f,0.0f, center.x + hx,center.y + hy,center.z - hz, 0.0f,1.0f,0.0f, center.x + hx,center.y + hy,center.z + hz, 0.0f,1.0f,0.0f, center.x - hx,center.y + hy,center.z - hz, 0.0f,1.0f,0.0f, center.x - hx,center.y + hy,center.z + hz, 0.0f,1.0f,0.0f });
}

void Player::setupStickmanModel() {
    std::vector<float> stickmanVertexData;
    float headSize = 0.3f;
    float torsoHeight_local = 0.7f, torsoWidth = 0.25f, torsoDepth = 0.1f;
    float armLength_local = 0.4f, armWidth = 0.08f;
    float legLength_local = 0.6f, legWidth = 0.1f; 

    addCubeVertices(stickmanVertexData, glm::vec3(0.0f, 0.0f, 0.0f), torsoWidth, torsoHeight_local, torsoDepth);
    addCubeVertices(stickmanVertexData, glm::vec3(0.0f, torsoHeight_local / 2.0f + headSize / 2.0f, 0.0f), headSize, headSize, headSize);
    addCubeVertices(stickmanVertexData, glm::vec3(-torsoWidth / 2.0f - armLength_local / 2.0f, torsoHeight_local / 2.0f - armWidth / 2.0f, 0.0f), armLength_local, armWidth, armWidth);
    addCubeVertices(stickmanVertexData, glm::vec3(torsoWidth / 2.0f + armLength_local / 2.0f, torsoHeight_local / 2.0f - armWidth / 2.0f, 0.0f), armLength_local, armWidth, armWidth);
    addCubeVertices(stickmanVertexData, glm::vec3(-torsoWidth / 2.0f + legWidth / 2.0f, -torsoHeight_local / 2.0f - legLength_local / 2.0f, 0.0f), legWidth, legLength_local, legWidth);
    addCubeVertices(stickmanVertexData, glm::vec3(torsoWidth / 2.0f - legWidth / 2.0f, -torsoHeight_local / 2.0f - legLength_local / 2.0f, 0.0f), legWidth, legLength_local, legWidth);

    vertexCount = stickmanVertexData.size() / 6;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, stickmanVertexData.size() * sizeof(float), stickmanVertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // std::cout << "[Player] Stickman model created. Vertices: " << vertexCount << std::endl;
}

Player::Player()
    : position(0.0f, 1.0f, 0.0f), 
    velocity(0.0f),
    jumpStrength(0.22f), // Zwiększona siła skoku dla "matriksowego" efektu
    moveSpeed(5.0f),    
    isGrounded(false), 
    VAO(0), VBO(0), vertexCount(0)
{
    setupStickmanModel();
}

Player::~Player() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // std::cout << "[Player] Stickman model destroyed." << std::endl;
}

void Player::Jump() {
    if (isGrounded) { 
        // std::cout << "[Player] Jump! Initial velocity.y: " << jumpStrength << std::endl;
        velocity.y = jumpStrength;
        isGrounded = false; 
    }
}

void Player::MoveRight(float deltaTime) {
    position.x += moveSpeed * deltaTime;
}

void Player::MoveLeft(float deltaTime) {
    position.x -= moveSpeed * deltaTime;
}

void Player::Update(float deltaTime) {
    if (!isGrounded || velocity.y > 0) { // Grawitacja działa jeśli nie na ziemi lub wznosi się
        float gravityConstant = 0.6f; // Nieco mocniejsza grawitacja
        velocity.y -= gravityConstant * deltaTime;
    }

    position += velocity * deltaTime;

    float playerFeetY = position.y - (getSize().y / 2.0f);
    float deathGroundLevel = -30.0f; // Niżej, bo budynki mogą być wysokie

    if (playerFeetY <= deathGroundLevel && velocity.y < 0) {
        // std::cout << "[Player] Fell to death ground! Resetting to a high position." << std::endl;
        // Resetuj gracza na bezpiecznej wysokości, np. na ostatniej znanej platformie lub wysoko w górze
        // Ta logika powinna być bardziej zaawansowana, np. reset do ostatniego checkpointu
        // Na razie, dla prostoty, resetujemy go wysoko, aby mógł spróbować wylądować.
        SetFullPosition(glm::vec3(position.x, 20.0f, position.z)); 
        velocity.y = 0.0f;
        isGrounded = false; // Jest w powietrzu po resecie
    }
}
void Player::Draw(Shader& shader) { 
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

    float stickmanOverallScale = 0.8f;
    modelMatrix = glm::scale(modelMatrix, glm::vec3(stickmanOverallScale));

    shader.setMat4("model", modelMatrix);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount));
    glBindVertexArray(0);
}
