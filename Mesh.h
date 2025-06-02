#pragma once // Upewnij si�, �e to jest na samej g�rze

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shader.h" // Mesh u�ywa Shader

// --- JEDYNA DEFINICJA STRUKTURY VERTEX ---
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords; // Zachowujemy, nawet je�li nie zawsze u�ywane przez wszystkie modele
};

// --- JEDYNA DEFINICJA STRUKTURY TEXTURE ---
// Ta struktura b�dzie u�ywana, je�li zdecydujesz si� doda� modele z teksturami
struct Texture {
    unsigned int id;
    std::string type; // np. "texture_diffuse", "texture_specular"
    std::string path; // �cie�ka do pliku tekstury (u�ywane do unikania duplikat�w)
};

class Mesh {
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures; // Lista tekstur u�ywanych przez t� siatk�
    unsigned int VAO;

    // Konstruktor przyjmuj�cy wszystkie dane, w tym tekstury
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);

    // Mo�esz doda� drugi konstruktor, je�li cz�sto tworzysz siatki bez tekstur:
    // Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    void Draw(Shader& shader);

private:
    unsigned int VBO, EBO;
    void setupMesh();
};