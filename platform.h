#pragma once

#include <glm/glm.hpp>
#include "Shader.h" // Potrzebne dla metody Draw
#include <vector>    // Potrzebne dla std::vector w implementacji
#include <GL/glew.h> // Potrzebne dla unsigned int VAO, VBO

class Platform {
public:
    // Konstruktor przyjmuje pozycj� �rodka platformy oraz jej pe�ne wymiary (szeroko��, wysoko��, g��boko��)
    Platform(glm::vec3 center_position, glm::vec3 dimensions);
    ~Platform();

    void Draw(Shader& shader);
    glm::vec3 getPosition() const { return position; } // Zwraca pozycj� �rodka platformy
    glm::vec3 getSize() const { return size; }       // Zwraca pe�ne wymiary platformy (szer, wys, g��b)

private:
    glm::vec3 position; // Pozycja �rodka platformy
    glm::vec3 size;     // Pe�ne wymiary platformy (szeroko��_X, wysoko��_Y, g��boko��_Z)

    unsigned int VAO, VBO;
    size_t vertexCount;

    void setupPlatformModel(); // Metoda do tworzenia geometrii prostopad�o�cianu
};