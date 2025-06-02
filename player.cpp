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
    // Wymiary bazowe patyczaka - te same warto�ci powinny by� u�yte do obliczenia baseHeight itp. w Player::getSize()
    float headSize = 0.3f;
    float torsoHeight_local = 0.7f, torsoWidth = 0.25f, torsoDepth = 0.1f;
    float armLength_local = 0.4f, armWidth = 0.08f;
    float legLength_local = 0.6f, legWidth = 0.1f; // Ta zmienna jest LOKALNA, u�ywana tylko do definicji kszta�tu

    // Pozycja (0,0,0) dla setupStickmanModel to �rodek (biodra) patyczaka.
    // Tu��w - jego �rodek Y jest na lokalnym Y=0
    addCubeVertices(stickmanVertexData, glm::vec3(0.0f, 0.0f, 0.0f), torsoWidth, torsoHeight_local, torsoDepth);
    // G�owa - nad tu�owiem
    addCubeVertices(stickmanVertexData, glm::vec3(0.0f, torsoHeight_local / 2.0f + headSize / 2.0f, 0.0f), headSize, headSize, headSize);
    // Lewe rami�
    addCubeVertices(stickmanVertexData, glm::vec3(-torsoWidth / 2.0f - armLength_local / 2.0f, torsoHeight_local / 2.0f - armWidth / 2.0f, 0.0f), armLength_local, armWidth, armWidth);
    // Prawe rami�
    addCubeVertices(stickmanVertexData, glm::vec3(torsoWidth / 2.0f + armLength_local / 2.0f, torsoHeight_local / 2.0f - armWidth / 2.0f, 0.0f), armLength_local, armWidth, armWidth);
    // Lewa noga - jej g�rna cz�� zaczyna si� od do�u tu�owia
    addCubeVertices(stickmanVertexData, glm::vec3(-torsoWidth / 2.0f + legWidth / 2.0f, -torsoHeight_local / 2.0f - legLength_local / 2.0f, 0.0f), legWidth, legLength_local, legWidth);
    // Prawa noga
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
    // Je�li Tw�j basic.vert oczekuje TexCoords na location = 2, a patyczak ich nie ma,
    // mo�esz tu wys�a� sta�� warto�� (0,0) lub wy��czy� ten atrybut,
    // albo (najlepiej) doda� TexCoords (np. (0,0)) do danych wierzcho�k�w patyczaka.
    // Na razie zak�adamy, �e basic.frag z objectColor ignoruje TexCoords_pass.
    // Je�li basic.vert ma `layout (location = 2) in vec2 aTexCoords;` to ten atrybut musi by� w��czony.
    // Mo�na doda� dummy dane TexCoords (0,0) do addCubeVertices, je�li VBO ma by� kompletne.
    // Proste rozwi�zanie je�li nie ma danych TexCoords w VBO:
    // glVertexAttrib2f(2, 0.0f, 0.0f); // Ustawia sta�� warto�� dla atrybutu 2
    // glDisableVertexAttribArray(2); // A potem go wy��cz, je�li nie jest cz�ci� VBO
    // LUB, je�li Vertex struct zawsze ma TexCoords (nawet (0,0) dla patyczaka), to nic nie trzeba zmienia� w glVertexAttribPointer.

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    std::cout << "[Player] Stickman model created. Vertices: " << vertexCount << std::endl;
}

Player::Player()
    : position(0.0f, 1.0f, 0.0f), // Domy�lna pozycja startowa, ZOSTANIE NADPISANA W MAIN.CPP
    velocity(0.0f),
    jumpStrength(0.15f), // <<<< DOSTOSUJ SI�� SKOKU
    moveSpeed(4.0f),    // <<<< DOSTOSUJ PR�DKO�� RUCHU
    isJumping(false),
    VAO(0), VBO(0), vertexCount(0)
{
    setupStickmanModel();
}

Player::~Player() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    std::cout << "[Player] Stickman model destroyed." << std::endl;
}

// Player.cpp
// ... (includes, addCubeVertices, setupStickmanModel, konstruktor, destruktor - bez zmian z ostatniej pe�nej wersji) ...

void Player::Jump() {
    // Na razie zostawiamy warunek - je�li kolizja nie dzia�a, to mo�e by� problem
    if (!isJumping) {
        std::cout << "[Player] Jump! Initial velocity.y: " << jumpStrength << std::endl;
        velocity.y = jumpStrength;
        isJumping = true;
    }
    else {
        // std::cout << "[Player] Tried to jump, but already jumping." << std::endl;
    }
}

void Player::MoveRight(float deltaTime) {
    position.x += moveSpeed * deltaTime;
}

void Player::MoveLeft(float deltaTime) {
    position.x -= moveSpeed * deltaTime;
}

void Player::Update(float deltaTime) {
    // --- GRAWITACJA ---
    float gravityConstant = 0.5f; // <<<< SPR�BUJ ZWI�KSZY� T� WARTO��, np. 0.5f, 0.8f, lub nawet 1.0f dla testu
    velocity.y -= gravityConstant * deltaTime;

    // --- AKTUALIZACJA POZYCJI ---
    position += velocity * deltaTime;

    // --- LOGOWANIE STANU GRACZA ---
    // Odkomentuj, aby zobaczy� warto�ci w ka�dej klatce
    /*
    std::cout << std::fixed << std::setprecision(3)
              << "Player Update - PosY: " << position.y
              << " VelY: " << velocity.y
              << " isJumping: " << (isJumping ? "T" : "F")
              << " FeetY: " << (position.y - getSize().y / 2.0f)
              << std::endl;
    */

    // Kolizja z "pod�og� �mierci" - bardzo nisko, aby nie przeszkadza� platformom
    // Ta kolizja powinna dzia�a� niezale�nie od platform, jako ostateczne zabezpieczenie.
    float playerFeetY = position.y - (getSize().y / 2.0f);
    float deathGroundLevel = -20.0f; // Ustaw odpowiednio nisko

    if (playerFeetY <= deathGroundLevel && velocity.y < 0) {
        std::cout << "[Player] Fell to death ground! Resetting." << std::endl;
        SetFullPosition(glm::vec3(position.x, deathGroundLevel + (getSize().y / 2.0f), position.z)); // Resetuj na tej pod�odze
        velocity.y = 0.0f;
        isJumping = false; // Wyl�dowa� (na pod�odze �mierci)
    }
}
void Player::Draw(Shader& shader) { // Upewnij si�, �e ta metoda istnieje i ma cia�o
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

    float stickmanOverallScale = 0.8f;
    modelMatrix = glm::scale(modelMatrix, glm::vec3(stickmanOverallScale));

    shader.setMat4("model", modelMatrix);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount));
    glBindVertexArray(0);
}