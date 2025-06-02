#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Player.h"    
#include "Platform.h"  
#include "stb_image.h"          

#include <vector>
#include <deque> // Do nieskończonego generowania platform
#include <iostream>
#include <string>
#include <iomanip> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib> // Dla std::rand i std::srand
#include <ctime>   // Dla std::time

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Player* player = nullptr;
// std::vector<Platform*> platforms; // Zmienione na deque
std::deque<Platform*> platforms_deque; 
float last_platform_x = 0.0f;       
float last_platform_y = 0.0f; // Do śledzenia wysokości ostatniej platformy
const int INITIAL_PLATFORMS = 8;    // Ile platform generujemy na start
const float PLATFORM_VIEW_DISTANCE_POSITIVE_X = 30.0f; // Jak daleko w prawo generujemy
const float PLATFORM_REMOVAL_DISTANCE_NEGATIVE_X = -40.0f; // Jak daleko w lewo usuwamy

// Parametry generowania platform
const float MIN_PLATFORM_SPACING_X = 2.5f;
const float MAX_PLATFORM_SPACING_X = 5.0f;
const float MIN_PLATFORM_HEIGHT_DIFFERENCE = -3.0f;
const float MAX_PLATFORM_HEIGHT_DIFFERENCE = 3.0f;
const float MIN_PLATFORM_WIDTH = 1.5f;
const float MAX_PLATFORM_WIDTH = 4.0f;
const float MIN_PLATFORM_HEIGHT = 10.0f;
const float MAX_PLATFORM_HEIGHT = 25.0f;
const float BASE_PLATFORM_Y = 0.0f; // Podstawa, od której liczymy wysokość platform

float lastFrame = 0.0f;
glm::vec3 globalLightPosition = glm::vec3(20.0f, 50.0f, 20.0f); // Światło nieco wyżej
unsigned int skyboxVAO = 0, skyboxVBO = 0;
unsigned int cubemapTextureID = 0;

void processInput(GLFWwindow* window, float deltaTime);
// bool checkCollision(glm::vec3 pos1, glm::vec3 size1, glm::vec3 pos2, glm::vec3 size2); // Już niepotrzebne w tej formie
unsigned int loadCubemap(const std::vector<std::string>& faces);
void gameLogicAndCollisions(float deltaTime); // Deklaracja
void createNewPlatform(); // Deklaracja
void managePlatforms();   // Deklaracja

unsigned int loadCubemap(const std::vector<std::string>& faces) {
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
            GLenum internalFormat = (nrChannels == 4) ? GL_SRGB_ALPHA : GL_SRGB; // Użyj sRGB dla lepszych kolorów
            if (nrChannels == 1) internalFormat = GL_RED; // Jeśli tylko jeden kanał, to RED
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            // std::cout << "[Cubemap Load] SUCCESS: Loaded texture " << faces[i] << std::endl;
            stbi_image_free(data);
        }
        else {
            std::cout << "[Cubemap Load] FAILED: Cubemap texture failed to load at path: " << faces[i] << std::endl;
            std::cout << "[Cubemap Load] STB Reason: " << stbi_failure_reason() << std::endl;
            glDeleteTextures(1, &textureID_cubemap);
            return 0;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Lepsze filtrowanie
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP); // Generuj mipmapy dla skyboxa
    return textureID_cubemap;
}

// Funkcja pomocnicza do generowania losowej liczby float w zakresie
float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void createNewPlatform() {
    float new_x;
    float new_y_offset;
    glm::vec3 new_dimensions;

    if (platforms_deque.empty()) {
        new_x = 0.0f; // Pierwsza platforma
        last_platform_y = BASE_PLATFORM_Y + MIN_PLATFORM_HEIGHT / 2.0f; // Ustaw wysokość pierwszej platformy
        new_dimensions = glm::vec3(randomFloat(MIN_PLATFORM_WIDTH, MAX_PLATFORM_WIDTH), 
                                   MIN_PLATFORM_HEIGHT, // Pierwsza platforma o minimalnej wysokości
                                   randomFloat(MIN_PLATFORM_WIDTH, MAX_PLATFORM_WIDTH)); // Głębokość Z
    } else {
        Platform* lastPtf = platforms_deque.back();
        new_x = lastPtf->getPosition().x + lastPtf->getSize().x / 2.0f + // Koniec ostatniej platformy
                randomFloat(MIN_PLATFORM_SPACING_X, MAX_PLATFORM_SPACING_X); // Odstęp
        
        new_y_offset = randomFloat(MIN_PLATFORM_HEIGHT_DIFFERENCE, MAX_PLATFORM_HEIGHT_DIFFERENCE);
        // Ogranicz zmianę wysokości, aby nie było zbyt dużych skoków
        float proposed_y = last_platform_y + new_y_offset;
        last_platform_y = glm::clamp(proposed_y, BASE_PLATFORM_Y - 5.0f, BASE_PLATFORM_Y + 15.0f); // Ograniczenie Y

        new_dimensions = glm::vec3(randomFloat(MIN_PLATFORM_WIDTH, MAX_PLATFORM_WIDTH), 
                                   randomFloat(MIN_PLATFORM_HEIGHT, MAX_PLATFORM_HEIGHT),
                                   randomFloat(MIN_PLATFORM_WIDTH, MAX_PLATFORM_WIDTH)); // Głębokość Z
        new_x += new_dimensions.x / 2.0f; // Dodaj połowę szerokości nowej platformy
    }
    
    // Pozycja Y platformy to jej środek, więc dodajemy połowę jej wysokości do `last_platform_y`
    // które jest interpretowane jako górna powierzchnia poprzedniej platformy lub base_y.
    // Ale Platform konstruktor oczekuje środka, więc `last_platform_y` powinno być środkiem.
    // Uprośćmy: `last_platform_y` będzie środkiem Y ostatniej platformy.
    // Nowa platforma będzie miała środek Y = last_platform_y (środek poprzedniej) + new_y_offset.
    // A jej podstawa będzie na last_platform_y + new_y_offset - new_dimensions.y / 2.0f.
    // Upewnijmy się, że podstawa nie jest za nisko.
    float platform_center_y = last_platform_y + new_y_offset;
    if (platform_center_y - new_dimensions.y / 2.0f < BASE_PLATFORM_Y - 10.0f) { // Nie pozwól spaść za nisko
        platform_center_y = BASE_PLATFORM_Y - 10.0f + new_dimensions.y / 2.0f;
    }
    last_platform_y = platform_center_y; // Zapisz środek Y nowej platformy


    platforms_deque.push_back(new Platform(glm::vec3(new_x, platform_center_y, 0.0f), new_dimensions));
    last_platform_x = new_x + new_dimensions.x / 2.0f; // Zapisz prawą krawędź nowej platformy
    // std::cout << "[Platform Gen] Created new platform at X: " << new_x 
    //           << " Y: " << platform_center_y 
    //           << " W: " << new_dimensions.x << " H: " << new_dimensions.y << std::endl;
}


void managePlatforms() {
    if (!player) return;
    glm::vec3 playerPos = player->getPosition();

    // 1. Generowanie nowych platform, jeśli gracz zbliża się do prawej krawędzi widoku
    while (platforms_deque.empty() || last_platform_x < playerPos.x + PLATFORM_VIEW_DISTANCE_POSITIVE_X) {
        createNewPlatform();
    }

    // 2. Usuwanie starych platform, które są daleko za graczem (po lewej)
    while (!platforms_deque.empty()) {
        Platform* firstPtf = platforms_deque.front();
        // Usuwamy platformę, jeśli jej prawa krawędź jest daleko za lewą krawędzią widoku gracza
        if (firstPtf->getPosition().x + firstPtf->getSize().x / 2.0f < playerPos.x + PLATFORM_REMOVAL_DISTANCE_NEGATIVE_X) {
            // std::cout << "[Platform Rem] Removing platform at X: " << firstPtf->getPosition().x << std::endl;
            delete firstPtf;
            platforms_deque.pop_front();
        } else {
            break; // Pierwsza platforma jest wciąż widoczna, więc nie usuwamy dalej
        }
    }
}


int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // Inicjalizacja generatora liczb losowych

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Antialiasing

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Matrix Skakanie v2", NULL, NULL);
    if (window == NULL) { std::cerr << "Failed to create GLFW window" << std::endl; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) { std::cerr << "Failed to initialize GLEW" << std::endl; return -1; }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE); // Włącz multisampling dla antialiasingu
    glEnable(GL_CULL_FACE); // Optymalizacja: nie rysuj tylnych ścian
    // std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    Shader shader("shaders/basic.vert", "shaders/basic.frag");
    if (shader.ID == 0) { return -1; }
    Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");
    if (skyboxShader.ID == 0) { return -1; }

    float skyboxVertices[] = { 
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

    std::vector<std::string> faces = { 
        "textures/skybox/right.jpg", "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",   "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg", "textures/skybox/back.jpg"
    };
    cubemapTextureID = loadCubemap(faces);
    if (cubemapTextureID == 0) { std::cerr << "Cubemap texture failed." << std::endl; }
    skyboxShader.use(); skyboxShader.setInt("skyboxTextureSampler", 0);
    
    // Inicjalne generowanie platform
    for (int i = 0; i < INITIAL_PLATFORMS; ++i) {
        createNewPlatform();
    }

    player = new Player();

    if (!platforms_deque.empty()) {
        Platform* startPlatform = platforms_deque.front();
        glm::vec3 platformPos = startPlatform->getPosition(); 
        glm::vec3 platformSize = startPlatform->getSize();    
        glm::vec3 playerSize = player->getSize();
        float playerCenterY_start = (platformPos.y + platformSize.y / 2.0f) + (playerSize.y / 2.0f) + 0.1f; // +0.1f dla pewności
        player->SetFullPosition(glm::vec3(platformPos.x, playerCenterY_start, platformPos.z));
        player->setIsGrounded(true); // Gracz startuje na platformie
    } else { // Fallback, jeśli nie ma platform
        player->SetFullPosition(glm::vec3(0.0f, player->getSize().y / 2.0f + 5.0f, 0.0f));
    }


    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f); // Zwiększony far plane
    // std::cout << std::fixed << std::setprecision(2);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (deltaTime > 0.1f) deltaTime = 0.1f; // Ograniczenie deltaTime
        if (deltaTime <= 0.0f) deltaTime = 1.0f / 120.0f; // Minimalny deltaTime

        processInput(window, deltaTime);
        
        player->Update(deltaTime);
        gameLogicAndCollisions(deltaTime); // Logika gry i kolizji
        managePlatforms(); // Zarządzanie platformami

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f); // Ciemniejsze tło
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 cameraOffset = glm::vec3(0.0f, 7.0f, 20.0f); // Kamera nieco wyżej i dalej
        glm::vec3 currentPlayerPosition = player ? player->getPosition() : glm::vec3(0.0f);
        glm::vec3 cameraPosition = currentPlayerPosition + cameraOffset;
        glm::vec3 cameraTarget = currentPlayerPosition + glm::vec3(0.0f, 1.0f, 0.0f); // Cel kamery nieco wyżej niż stopy gracza
        glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));

        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setVec3("viewPos", cameraPosition);
        shader.setVec3("light.position", globalLightPosition);
        shader.setVec3("light.ambient", glm::vec3(0.4f)); // Nieco jaśniejsze światło otoczenia
        shader.setVec3("light.diffuse", glm::vec3(0.8f));
        shader.setVec3("light.specular", glm::vec3(0.6f));
        shader.setFloat("material_shininess", 64.0f); // Większy połysk

        // Kolor dla platform - można by go zmieniać losowo lub na podstawie odległości
        shader.setVec3("objectColor", glm::vec3(0.2f, 0.25f, 0.3f)); 
        for (auto& platform_ptr : platforms_deque) { // Iteruj po deque
            if (platform_ptr) platform_ptr->Draw(shader);
        }

        if (player) {
            shader.setVec3("objectColor", glm::vec3(0.8f, 0.1f, 0.1f)); // Czerwony gracz
            player->Draw(shader);
        }

        if (cubemapTextureID != 0) { 
            glDepthFunc(GL_LEQUAL); // Zmień funkcję głębi dla skyboxa
            skyboxShader.use();
            glm::mat4 viewSky = glm::mat4(glm::mat3(view)); // Usuń translację z macierzy widoku
            skyboxShader.setMat4("view", viewSky);
            skyboxShader.setMat4("projection", projection);
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS); // Przywróć domyślną funkcję głębi
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &skyboxVAO); glDeleteBuffers(1, &skyboxVBO);
    if (cubemapTextureID != 0) glDeleteTextures(1, &cubemapTextureID);
    delete player; player = nullptr;
    for (auto& platform_ptr : platforms_deque) { delete platform_ptr; } // Usuń platformy z deque
    platforms_deque.clear();
    
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window, float deltaTime) { 
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (player) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) player->Jump();
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) player->MoveLeft(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) player->MoveRight(deltaTime);
    }
}

void gameLogicAndCollisions(float deltaTime) {
    if (!player) return;

    bool onAnyPlatformThisFrame = false;
    glm::vec3 playerPos = player->getPosition(); 
    glm::vec3 playerSize = player->getSize(); 
    float playerFeetWorldY = playerPos.y - playerSize.y / 2.0f; 

    for (auto& ptf : platforms_deque) { // Iteruj po deque
        if (!ptf) continue;

        glm::vec3 platformPos = ptf->getPosition(); 
        glm::vec3 platformSize = ptf->getSize(); 
        float platformTopWorldY = platformPos.y + platformSize.y / 2.0f; 

        bool collisionX = (playerPos.x - playerSize.x / 2.0f < platformPos.x + platformSize.x / 2.0f) &&
                          (playerPos.x + playerSize.x / 2.0f > platformPos.x - platformSize.x / 2.0f); 
        bool collisionZ = (playerPos.z - playerSize.z / 2.0f < platformPos.z + platformSize.z / 2.0f) &&
                          (playerPos.z + playerSize.z / 2.0f > platformPos.z - platformSize.z / 2.0f); 

        if (collisionX && collisionZ) {
            float landingPrecision = 0.18f; // Zwiększony margines na lądowanie, aby było łatwiej
            
            // Gracz ląduje, jeśli spada (velocity.y <= 0)
            // i jego stopy są BARDZO blisko górnej powierzchni platformy
            // (lub minimalnie ją przeniknęły z powodu kroku czasowego)
            if (player->getVelocity().y <= 0.01f && // Gracz spada lub prawie stoi
                playerFeetWorldY >= platformTopWorldY - landingPrecision &&
                playerFeetWorldY <= platformTopWorldY + (std::abs(player->getVelocity().y * deltaTime) + 0.02f)) // Mały bufor na przenikanie
            {
                // std::cout << "======= LANDED on Platform! PlayerFeetY: " << playerFeetWorldY << " PlatformTopY: " << platformTopWorldY << " VelY: " << player->getVelocity().y << " =======" << std::endl;
                player->setPositionY(platformTopWorldY + playerSize.y / 2.0f); 
                player->setVelocityY(0.0f);    
                player->setIsGrounded(true);  
                onAnyPlatformThisFrame = true;
                break; 
            }
        }
    }

    if (!onAnyPlatformThisFrame && player->getIsGrounded() && player->getVelocity().y <= 0) {
        // std::cout << "======= Player is falling off a platform! =======" << std::endl;
        player->setIsGrounded(false); 
    }
}
