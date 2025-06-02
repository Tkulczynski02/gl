#pragma once

#include <glm/glm.hpp>
#include "Shader.h" // Potrzebne dla metody Draw
#include <vector>    // Potrzebne dla std::vector w implementacji
#include <GL/glew.h> // Potrzebne dla unsigned int VAO, VBO

class Platform {
public:
    // Konstruktor przyjmuje pozycjê œrodka platformy oraz jej pe³ne wymiary (szerokoœæ, wysokoœæ, g³êbokoœæ)
    Platform(glm::vec3 center_position, glm::vec3 dimensions);
    ~Platform();

    void Draw(Shader& shader);
    glm::vec3 getPosition() const { return position; } // Zwraca pozycjê œrodka platformy
    glm::vec3 getSize() const { return size; }       // Zwraca pe³ne wymiary platformy (szer, wys, g³êb)

private:
    glm::vec3 position; // Pozycja œrodka platformy
    glm::vec3 size;     // Pe³ne wymiary platformy (szerokoœæ_X, wysokoœæ_Y, g³êbokoœæ_Z)

    unsigned int VAO, VBO;
    size_t vertexCount;

    void setupPlatformModel(); // Metoda do tworzenia geometrii prostopad³oœcianu
};