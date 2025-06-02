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

    // === BARDZO WA¯NE: Dostosuj te wartoœci! ===
    // Zwraca wymiary patyczaka W ŒWIECIE GRY po jego przeskalowaniu.
    // (szerokoœæ X, wysokoœæ Y, g³êbokoœæ Z)
    glm::vec3 getSize() const {
        // Ta wartoœæ MUSI byæ taka sama jak `stickmanOverallScale` w Player::Draw()
        float currentStickmanScale = 0.8f; // <<<< Czy to jest ta sama wartoœæ, co w Player::Draw? SprawdŸ!

        // Wymiary bazowe patyczaka (jak zdefiniowane w setupStickmanModel)
        // SprawdŸ, czy te wartoœci s¹ dok³adnym odzwierciedleniem tego, co tworzy setupStickmanModel!
        float baseWidth = 0.3f;  // Szerokoœæ tu³owia. Mo¿e powinno byæ szersze, jeœli rêce odstaj¹?
        // np. torsoWidth (0.25) + 2 * (po³owa szerokoœci ramienia, jeœli odstaj¹)
        // Na razie 0.3f (torsoWidth z setupStickmanModel to 0.25f) jest OK jako przybli¿enie.

// Obliczanie baseHeight:
// W setupStickmanModel:
// torsoHeight_local = 0.7f (œrodek tu³owia na Y=0, wiêc tu³ów od -0.35 do +0.35)
// headSize = 0.3f (œrodek g³owy na Y = torsoH/2 + headS/2 = 0.35 + 0.15 = 0.5; g³owa od 0.35 do 0.65)
// legLength_local = 0.6f (góra nogi na -torsoH/2 = -0.35; dó³ nogi na -torsoH/2 - legL = -0.35 - 0.6 = -0.95)
// Ca³kowita wysokoœæ od stóp (-0.95) do czubka g³owy (0.65) to 0.65 - (-0.95) = 1.6f
        float baseHeight = 1.6f; // Zgodnie z obliczeniami powy¿ej dla Twoich wartoœci z setupStickmanModel
        // (Poprzednio mia³eœ 0.7 + 0.3 + 0.6 = 1.6, wiêc to jest OK)

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