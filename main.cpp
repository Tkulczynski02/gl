#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Player.h"    
#include "Platform.h"  // U¿ywa teraz nowego Platform.h/cpp
// Usuniêto: #include "Model.h" // Nie potrzebujemy ju¿ Model.h dla platform
// Jeœli masz osobny plik stb_image.cpp z #define STB_IMAGE_IMPLEMENTATION,
// to poni¿sz¹ liniê #define STB_IMAGE_IMPLEMENTATION usuñ.
// #define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"          

#include <vector>
#include <iostream>
#include <string>
#include <iomanip> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Player* player = nullptr;
std::vector<Platform*> platforms;
// Usuniêto: Model* buildingModel1 = nullptr; 

float lastFrame = 0.0f;
glm::vec3 globalLightPosition = glm::vec3(20.0f, 30.0f, 15.0f);
unsigned int skyboxVAO = 0, skyboxVBO = 0;
unsigned int cubemapTextureID = 0;

void processInput(GLFWwindow* window, float deltaTime);
bool checkCollision(glm::vec3 pos1, glm::vec3 size1, glm::vec3 pos2, glm::vec3 size2);
unsigned int loadCubemap(const std::vector<std::string>& faces);

unsigned int loadCubemap(const std::vector<std::string>& faces) { /* ... Implementacja jak poprzednio ... */
    unsigned int textureID_cubemap;
    glGenTextures(1, &textureID_cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID_cubemap);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < faces.size(); i++) {
        if (i >= 6) break;
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = GL_RGB;
            if (nrChannels == 4) format = GL_RGBA;
            else if (nrChannels == 3) format = GL_RGB;
            else if (nrChannels == 1) format = GL_RED;
            else { std::cout << "[Cubemap Load] WARN: Unsupported #channels: " << nrChannels << " for " << faces[i] << std::endl; stbi_image_free(data); continue; }
            GLenum internalFormat = (nrChannels == 4) ? GL_SRGB_ALPHA : GL_SRGB;
            if (nrChannels == 1) internalFormat = GL_RED;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            std::cout << "[Cubemap Load] SUCCESS: Loaded texture " << faces[i] << std::endl;
            stbi_image_free(data);
        }
        else {
            std::cout << "[Cubemap Load] FAILED: Cubemap texture failed to load at path: " << faces[i] << std::endl;
            std::cout << "[Cubemap Load] STB Reason: " << stbi_failure_reason() << std::endl;
            glDeleteTextures(1, &textureID_cubemap);
            return 0;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    return textureID_cubemap;
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Skakanie po dachach vNEW", NULL, NULL);
    if (window == NULL) { /* ... */ return -1; }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) { /* ... */ return -1; }

    glEnable(GL_DEPTH_TEST);
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    Shader shader("shaders/basic.vert", "shaders/basic.frag");
    if (shader.ID == 0) { /* ... */ return -1; }
    Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");
    if (skyboxShader.ID == 0) { /* ... */ return -1; }

    float skyboxVertices[] = { /* ... (jak poprzednio) ... */
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
    };
    glGenVertexArrays(1, &skyboxVAO); glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO); glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    std::vector<std::string> faces = { /* ... œcie¿ki do tekstur skyboxa ... */
        "textures/skybox/right.jpg", "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",   "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg", "textures/skybox/back.jpg"
    };
    cubemapTextureID = loadCubemap(faces);
    if (cubemapTextureID == 0) { std::cerr << "Cubemap texture failed." << std::endl; }
    skyboxShader.use(); skyboxShader.setInt("skyboxTextureSampler", 0);

    // Usuniêto ³adowanie buildingModel1 z pliku, bo platformy generuj¹ w³asn¹ geometriê

    // --- Tworzenie platform z nowym konstruktorem Platform ---
    // Definiujemy wymiary platformy (szerokoœæ X, wysokoœæ Y, g³êbokoœæ Z)
    glm::vec3 platformDimensions = glm::vec3(2.0f, 15.0f, 2.0f); // <<<< DOSTOSUJ WYMIARY BUDYNKÓW
    // Te wymiary bêd¹ u¿ywane w Platform::setupPlatformModel
    // oraz zwracane przez Platform::getSize()

// Pozycja Y podstawy (lub œrodka, zale¿nie od tego jak zdefiniujesz Platform::position)
    float commonYPosition_platforms = platformDimensions.y / 2.0f; // Jeœli chcemy, aby podstawa platform by³a na Y=0
    // to œrodek platformy musi byæ na Y = wysokoœæ/2
    // Dostosuj, jeœli chcesz obni¿yæ/podnieœæ wszystkie
// float commonYPosition_platforms = 0.0f; // Jeœli Platform::position to œrodek, a chcesz je na ziemi

    float commonZPosition_platforms = 0.0f;
    float spacingX = platformDimensions.x + 2.0f; // Odstêp = szerokoœæ platformy + trochê przerwy
    // <<<< DOSTOSUJ spacingX

    platforms.push_back(new Platform(glm::vec3(-1.5f * spacingX, commonYPosition_platforms, commonZPosition_platforms), platformDimensions));
    platforms.push_back(new Platform(glm::vec3(-0.5f * spacingX, commonYPosition_platforms, commonZPosition_platforms), platformDimensions));
    platforms.push_back(new Platform(glm::vec3(0.5f * spacingX, commonYPosition_platforms, commonZPosition_platforms), platformDimensions));
    platforms.push_back(new Platform(glm::vec3(1.5f * spacingX, commonYPosition_platforms, commonZPosition_platforms), platformDimensions));

    player = new Player();

    // Ustawienie pozycji startowej gracza na jednej z platform
    int startPlatformIndex = 1;
    if (platforms.size() > static_cast<size_t>(startPlatformIndex) && platforms[startPlatformIndex] != nullptr) {
        Platform* startPlatform = platforms[startPlatformIndex];
        glm::vec3 platformPos = startPlatform->getPosition(); // Œrodek Y platformy
        glm::vec3 platformSize = startPlatform->getSize();    // = platformDimensions
        glm::vec3 playerSize = player->getSize();

        float playerCenterY_start = (platformPos.y + platformSize.y / 2.0f) + (playerSize.y / 2.0f);
        player->SetFullPosition(glm::vec3(platformPos.x, playerCenterY_start, platformPos.z));
        std::cout << "[Main] Player starting. PlatformCenter_Y: " << platformPos.y
            << ", PlatformTop_Y: " << platformPos.y + platformSize.y / 2.0f
            << ", Player_TargetCenter_Y: " << playerCenterY_start
            << std::endl;
    }
    else {
        player->SetFullPosition(glm::vec3(0.0f, player->getSize().y / 2.0f, 0.0f));
    }

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
    std::cout << std::fixed << std::setprecision(2);

    while (!glfwWindowShouldClose(window)) {
        // ... (deltaTime, processInput, glClear - bez zmian) ...
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        if (deltaTime <= 0.0f) deltaTime = 1.0f / 120.0f;
        processInput(window, deltaTime);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 cameraOffset = glm::vec3(0.0f, 6.0f, 18.0f);
        glm::vec3 currentPlayerPosition = player ? player->getPosition() : glm::vec3(0.0f);
        glm::vec3 cameraPosition = currentPlayerPosition + cameraOffset;
        glm::vec3 cameraTarget = currentPlayerPosition + glm::vec3(0.0f, 0.8f, 0.0f);
        glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));

        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setVec3("viewPos", cameraPosition);
        shader.setVec3("light.position", globalLightPosition);
        shader.setVec3("light.ambient", glm::vec3(0.35f));
        shader.setVec3("light.diffuse", glm::vec3(0.75f));
        shader.setVec3("light.specular", glm::vec3(0.5f));
        shader.setFloat("material_shininess", 32.0f);

        shader.setVec3("objectColor", glm::vec3(0.5f, 0.55f, 0.6f)); // Kolor dla platform
        for (auto& platform_ptr : platforms) {
            if (platform_ptr) platform_ptr->Draw(shader);
        }

        if (player) {
            player->Update(deltaTime);

            // W main.cpp, w pêtli while
             // ... (po player->Update(deltaTime);) ...
            bool onAnyPlatform = false;
            if (player) { // Upewnij siê, ¿e player nie jest nullptr
                for (auto& ptf : platforms) {
                    if (ptf) {
                        glm::vec3 playerPos = player->getPosition();
                        glm::vec3 playerSize = player->getSize();
                        glm::vec3 platformPos = ptf->getPosition();
                        glm::vec3 platformSize = ptf->getSize();

                        float playerFeetWorldY = playerPos.y - playerSize.y / 2.0f;
                        float platformTopWorldY = platformPos.y + platformSize.y / 2.0f;

                        bool collisionX = (playerPos.x - playerSize.x / 2.0f < platformPos.x + platformSize.x / 2.0f) &&
                            (playerPos.x + playerSize.x / 2.0f > platformPos.x - platformSize.x / 2.0f);
                        bool collisionZ = (playerPos.z - playerSize.z / 2.0f < platformPos.z + platformSize.z / 2.0f) &&
                            (playerPos.z + playerSize.z / 2.0f > platformPos.z - platformSize.z / 2.0f);


                        if (ptf == platforms[startPlatformIndex]) { // Loguj np. tylko dla platformy startowej
                             std::cout << std::fixed << std::setprecision(3)
                                       << "P_Y: " << playerPos.y << " P_FeetY: " << playerFeetWorldY << " P_VelY: " << player->getVelocity().y
                                       << " | Plat_Y: " << platformPos.y << " Plat_TopY: " << platformTopWorldY
                                       << " | PSizeY: " << playerSize.y << " PlatSizeY: " << platformSize.y
                                       << " | OverlapXZ: " << (collisionX ? "T" : "F") << (collisionZ ? "T" : "F")
                                       << std::endl;
                        }

                        if (collisionX && collisionZ) {
                            float landingPrecision = 0.15f;
                            float passThroughBuffer = 0.05f;  // Zmniejszy³em ten bufor, aby byæ bardziej precyzyjnym

                            // Gracz spada lub prawie stoi (ma³a prêdkoœæ pionowa LUB jest ju¿ bardzo blisko góry platformy)
                            // ORAZ jego stopy s¹ "w kontakcie" z górn¹ powierzchni¹ platformy (z marginesami)
                            if (player->getVelocity().y <= 0.01f &&  // Gracz spada lub prawie nie ma prêdkoœci pionowej
                                playerFeetWorldY >= platformTopWorldY - landingPrecision &&
                                playerFeetWorldY <= platformTopWorldY + (std::abs(player->getVelocity().y * deltaTime) + passThroughBuffer))
                            {
                                std::cout << "======= LANDED on Platform! =======" << std::endl; // WA¯NE: SprawdŸ, czy ten log siê pojawia
                                player->setPositionY(platformTopWorldY + playerSize.y / 2.0f);
                                player->setVelocityY(0.0f);    // ZERUJEMY PRÊDKOŒÆ Y
                                player->setIsJumping(false);   // USTAWIAMY, ¯E NIE SKACZE
                                onAnyPlatform = true;
                                break;
                            }
                        }
                    }
                } // koniec if (ptf)

                // Jeœli gracz nie jest na ¿adnej platformie po sprawdzeniu wszystkich,
                // a wczeœniej nie skaka³ (np. spad³ z krawêdzi), upewnij siê, ¿e grawitacja zadzia³a.
                // Warunek `if (isJumping || velocity.y < 0)` w Player::Update powinien to obs³u¿yæ.
                // Mo¿na by tu ewentualnie ustawiæ player->setIsJumping(true), jeœli nie onAnyPlatform i velocity.y < 0,
                // aby zapobiec skokowi od razu po spadniêciu, ale to mo¿e byæ myl¹ce.

            } // koniec pêtli po platformach
            // ... (rysowanie gracza) ...
            shader.setVec3("objectColor", glm::vec3(0.9f, 0.2f, 0.2f));
            player->Draw(shader);
        }

        if (cubemapTextureID != 0) { /* ... rysowanie skyboxa bez zmian ... */
            glDepthFunc(GL_LEQUAL);
            skyboxShader.use();
            glm::mat4 viewSky = glm::mat4(glm::mat3(view));
            skyboxShader.setMat4("view", viewSky);
            skyboxShader.setMat4("projection", projection);
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- Zwalnianie zasobów ---
    glDeleteVertexArrays(1, &skyboxVAO); glDeleteBuffers(1, &skyboxVBO);
    if (cubemapTextureID != 0) glDeleteTextures(1, &cubemapTextureID);
    delete player; player = nullptr;
    for (auto& platform_ptr : platforms) { delete platform_ptr; }
    platforms.clear();
    // Usuniêto delete buildingModel1, bo nie jest ju¿ ³adowany jako osobny zasób,
    // platformy s¹ samowystarczalne. Jeœli Model.h/cpp s¹ nadal w projekcie,
    // a nie tworzysz obiektów Model, to nie ma czego usuwaæ.
    // Jeœli jednak buildingModel1 by³o u¿ywane do czegoœ innego, przywróæ.
    // W tym kodzie, buildingModel1 nie jest ju¿ potrzebny, bo Platform tworzy w³asn¹ geometriê.
    // Zmieni³em kod tak, aby Platformy nie potrzebowa³y Model*.

    glfwTerminate();
    return 0;
}

// --- Definicje Funkcji Pomocniczych ---
void processInput(GLFWwindow* window, float deltaTime) { /* ... implementacja jak poprzednio ... */
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (player) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) player->Jump();
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) player->MoveLeft(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) player->MoveRight(deltaTime);
    }
}
bool checkCollision(glm::vec3 pos1, glm::vec3 size1, glm::vec3 pos2, glm::vec3 size2) { /* ... implementacja jak poprzednio ... */
    bool cX = (pos1.x - size1.x / 2.0f < pos2.x + size2.x / 2.0f) && (pos1.x + size1.x / 2.0f > pos2.x - size2.x / 2.0f);
    bool cY = (pos1.y - size1.y / 2.0f < pos2.y + size2.y / 2.0f) && (pos1.y + size1.y / 2.0f > pos2.y - size2.y / 2.0f);
    bool cZ = (pos1.z - size1.z / 2.0f < pos2.z + size2.z / 2.0f) && (pos1.z + size1.z / 2.0f > pos2.z - size2.z / 2.0f);
    return cX && cY && cZ;
}