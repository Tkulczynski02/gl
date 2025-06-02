#pragma once // Upewnij siê, ¿e to jest na samej górze

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shader.h" // Mesh u¿ywa Shader

// --- JEDYNA DEFINICJA STRUKTURY VERTEX ---
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords; // Zachowujemy, nawet jeœli nie zawsze u¿ywane przez wszystkie modele
};

// --- JEDYNA DEFINICJA STRUKTURY TEXTURE ---
// Ta struktura bêdzie u¿ywana, jeœli zdecydujesz siê dodaæ modele z teksturami
struct Texture {
    unsigned int id;
    std::string type; // np. "texture_diffuse", "texture_specular"
    std::string path; // Œcie¿ka do pliku tekstury (u¿ywane do unikania duplikatów)
};

class Mesh {
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures; // Lista tekstur u¿ywanych przez tê siatkê
    unsigned int VAO;

    // Konstruktor przyjmuj¹cy wszystkie dane, w tym tekstury
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);

    // Mo¿esz dodaæ drugi konstruktor, jeœli czêsto tworzysz siatki bez tekstur:
    // Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    void Draw(Shader& shader);

private:
    unsigned int VBO, EBO;
    void setupMesh();
};