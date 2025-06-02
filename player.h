#pragma once

#include <glm/glm.hpp>
#include "Shader.h" 

class Player {
public:
    Player();
    ~Player();

    void Update(float deltaTime);
    void Draw(Shader& shader);
    void Jump();
    void MoveRight(float deltaTime);
    void MoveLeft(float deltaTime);

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getVelocity() const { return velocity; }
    // bool getIsJumping() const { return isJumping; } // Usunięto
    bool getIsGrounded() const { return isGrounded; } // Nowa metoda

    void setPositionY(float y) { position.y = y; }
    void setVelocityY(float y_vel) { velocity.y = y_vel; }
    // void setIsJumping(bool jumping) { isJumping = jumping; } // Usunięto
    void setIsGrounded(bool grounded) { isGrounded = grounded; } // Nowa metoda
    void SetFullPosition(const glm::vec3& newPosition) { position = newPosition; }

    glm::vec3 getSize() const {
        float currentStickmanScale = 0.8f; 
        float baseWidth = 0.3f;  
        float baseHeight = 1.6f; 
        float baseDepth = 0.15f; 
        return glm::vec3(baseWidth * currentStickmanScale,
            baseHeight * currentStickmanScale,
            baseDepth * currentStickmanScale);
    }

private:
    glm::vec3 position;
    glm::vec3 velocity;
    float jumpStrength;
    float moveSpeed;
    // bool isJumping; // Zastąpione przez isGrounded
    bool isGrounded; // Nowa zmienna - czy gracz jest na ziemi

    unsigned int VAO, VBO;
    size_t vertexCount;

    void setupStickmanModel();
};
