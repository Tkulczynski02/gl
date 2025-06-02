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
    bool getIsJumping() const { return isJumping; }

    void setPositionY(float y) { position.y = y; }
    void setVelocityY(float y_vel) { velocity.y = y_vel; }
    void setIsJumping(bool jumping) { isJumping = jumping; }
    void SetFullPosition(const glm::vec3& newPosition) { position = newPosition; }

    // === BARDZO WA�NE: Dostosuj te warto�ci! ===
    // Zwraca wymiary patyczaka W �WIECIE GRY po jego przeskalowaniu.
    // (szeroko�� X, wysoko�� Y, g��boko�� Z)
    glm::vec3 getSize() const {
        // Ta warto�� MUSI by� taka sama jak `stickmanOverallScale` w Player::Draw()
        float currentStickmanScale = 0.8f; // <<<< Czy to jest ta sama warto��, co w Player::Draw? Sprawd�!

        // Wymiary bazowe patyczaka (jak zdefiniowane w setupStickmanModel)
        // Sprawd�, czy te warto�ci s� dok�adnym odzwierciedleniem tego, co tworzy setupStickmanModel!
        float baseWidth = 0.3f;  // Szeroko�� tu�owia. Mo�e powinno by� szersze, je�li r�ce odstaj�?
        // np. torsoWidth (0.25) + 2 * (po�owa szeroko�ci ramienia, je�li odstaj�)
        // Na razie 0.3f (torsoWidth z setupStickmanModel to 0.25f) jest OK jako przybli�enie.

// Obliczanie baseHeight:
// W setupStickmanModel:
// torsoHeight_local = 0.7f (�rodek tu�owia na Y=0, wi�c tu��w od -0.35 do +0.35)
// headSize = 0.3f (�rodek g�owy na Y = torsoH/2 + headS/2 = 0.35 + 0.15 = 0.5; g�owa od 0.35 do 0.65)
// legLength_local = 0.6f (g�ra nogi na -torsoH/2 = -0.35; d� nogi na -torsoH/2 - legL = -0.35 - 0.6 = -0.95)
// Ca�kowita wysoko�� od st�p (-0.95) do czubka g�owy (0.65) to 0.65 - (-0.95) = 1.6f
        float baseHeight = 1.6f; // Zgodnie z obliczeniami powy�ej dla Twoich warto�ci z setupStickmanModel
        // (Poprzednio mia�e� 0.7 + 0.3 + 0.6 = 1.6, wi�c to jest OK)

        float baseDepth = 0.15f; // torsoDepth z setupStickmanModel

        return glm::vec3(baseWidth * currentStickmanScale,
            baseHeight * currentStickmanScale,
            baseDepth * currentStickmanScale);
    }

private:
    glm::vec3 position;
    glm::vec3 velocity;
    float jumpStrength;
    float moveSpeed;
    bool isJumping;

    unsigned int VAO, VBO;
    size_t vertexCount;

    void setupStickmanModel();
};