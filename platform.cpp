#include "Platform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector> // Ju� do��czone w .h, ale dla jasno�ci

// Funkcja pomocnicza do dodawania wierzcho�k�w sze�cianu/prostopad�o�cianu
// (pozycja x,y,z, normalna nx,ny,nz) - taka sama jak w Player.cpp
// Ka�dy wierzcho�ek to 6 float�w: Px, Py, Pz, Nx, Ny, Nz
// Ta funkcja tworzy sze�cian/prostopad�o�cian o �rodku w (0,0,0) lokalnie,
// a potem przesuwamy go macierz� modelu w Draw().
// LUB: Mo�emy przekaza� wymiary do tej funkcji i ona od razu stworzy prostopad�o�cian o zadanych wymiarach.
// Wybierzmy drug� opcj� dla Platform, aby setupPlatformModel by� prostszy.

// Funkcja pomocnicza do dodawania wierzcho�k�w prostopad�o�cianu o zadanych wymiarach,
// wycentrowanego wok� lokalnego (0,0,0)
void addCuboidVertices(std::vector<float>& vertices, float width, float height, float depth) {
    float hx = width / 2.0f;
    float hy = height / 2.0f;
    float hz = depth / 2.0f;

    // Pozycje wzgl�dem lokalnego �rodka (0,0,0)
    // Prz�d (+Z) nx=0, ny=0, nz=1
    vertices.insert(vertices.end(), { -hx,-hy, hz, 0,0,1,  hx,-hy, hz, 0,0,1,  hx, hy, hz, 0,0,1,  hx, hy, hz, 0,0,1, -hx, hy, hz, 0,0,1, -hx,-hy, hz, 0,0,1 });
    // Ty� (-Z) nx=0, ny=0, nz=-1
    vertices.insert(vertices.end(), { -hx,-hy,-hz, 0,0,-1,  hx,-hy,-hz, 0,0,-1,  hx, hy,-hz, 0,0,-1,  hx, hy,-hz, 0,0,-1, -hx, hy,-hz, 0,0,-1, -hx,-hy,-hz, 0,0,-1 });
    // Lewo (-X) nx=-1, ny=0, nz=0
    vertices.insert(vertices.end(), { -hx,-hy,-hz, -1,0,0, -hx,-hy, hz, -1,0,0, -hx, hy, hz, -1,0,0, -hx, hy, hz, -1,0,0, -hx, hy,-hz, -1,0,0, -hx,-hy,-hz, -1,0,0 });
    // Prawo (+X) nx=1, ny=0, nz=0
    vertices.insert(vertices.end(), { hx,-hy,-hz,  1,0,0,  hx,-hy, hz,  1,0,0,  hx, hy, hz,  1,0,0,  hx, hy, hz,  1,0,0,  hx, hy,-hz,  1,0,0,  hx,-hy,-hz,  1,0,0 });
    // D� (-Y) nx=0, ny=-1, nz=0
    vertices.insert(vertices.end(), { -hx,-hy,-hz, 0,-1,0,  hx,-hy,-hz, 0,-1,0,  hx,-hy, hz, 0,-1,0,  hx,-hy, hz, 0,-1,0, -hx,-hy, hz, 0,-1,0, -hx,-hy,-hz, 0,-1,0 });
    // G�ra (+Y) nx=0, ny=1, nz=0
    vertices.insert(vertices.end(), { -hx, hy,-hz, 0,1,0,  hx, hy,-hz, 0,1,0,  hx, hy, hz, 0,1,0,  hx, hy, hz, 0,1,0, -hx, hy, hz, 0,1,0, -hx, hy,-hz, 0,1,0 });
}


Platform::Platform(glm::vec3 center_position, glm::vec3 dimensions)
    : position(center_position), size(dimensions), VAO(0), VBO(0), vertexCount(0)
{
    setupPlatformModel();
    std::cout << "[Platform] Created at (" << position.x << "," << position.y << "," << position.z << ")"
        << " with size (" << size.x << "," << size.y << "," << size.z << ")" << std::endl;
}

Platform::~Platform() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // std::cout << "[Platform] Destroyed." << std::endl;
}

void Platform::setupPlatformModel() {
    std::vector<float> platformVertexData;

    // U�yj wymiar�w platformy przekazanych do konstruktora
    addCuboidVertices(platformVertexData, this->size.x, this->size.y, this->size.z);

    vertexCount = platformVertexData.size() / 6; // 6 float�w na wierzcho�ek (3 pozycja + 3 normalna)

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, platformVertexData.size() * sizeof(float), platformVertexData.data(), GL_STATIC_DRAW);

    // Atrybut pozycji (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Atrybut normalnych (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Atrybut wsp�rz�dnych tekstury (location = 2)
    // Nawet je�li nie u�ywamy tekstur dla platform (u�ywamy objectColor),
    // nasz basic.vert mo�e oczekiwa� tego atrybutu.
    // Mo�emy wys�a� sta�e warto�ci (0,0) je�li dane nie s� w VBO,
    // lub doda� TexCoords do addCuboidVertices i tu je ustawi�.
    // Je�li basic.vert ma `layout (location = 2) in vec2 aTexCoords;` to musi by� on w��czony.
    // Proste rozwi�zanie, je�li Vertex struct ma TexCoords, a my ich nie ustawiamy w addCuboidVertices:
    // dane b�d� �mieciowe, ale je�li shader ich nie u�ywa, nie ma problemu.
    // Dla shadera z objectColor, TexCoords nie s� u�ywane w basic.frag.
    // Mo�na by doda� dummy TexCoords (0,0) do danych wierzcho�k�w (np. 8 float�w na wierzcho�ek)
    // i ustawi� tu glVertexAttribPointer dla location 2.
    // Na razie zak�adamy, �e basic.vert przekazuje aTexCoords, a basic.frag (z objectColor) je ignoruje.


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // std::cout << "[Platform] Model setup. Vertices: " << vertexCount << std::endl;
}


void Platform::Draw(Shader& shader) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // Platforma jest tworzona o swoich rzeczywistych wymiarach wok� lokalnego (0,0,0).
    // `this->position` to pozycja jej �rodka w �wiecie gry.
    modelMatrix = glm::translate(modelMatrix, this->position);

    // Rotacja, je�li potrzebna (np. dla budynk�w obr�conych o 90 stopni)
    float rotationAngleDegrees = 90.0f; // <<<< ZACHOWAJ LUB ZMIE�, je�li chcesz inn� orientacj�
    glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngleDegrees), rotationAxis);

    // Skalowanie nie jest ju� tutaj potrzebne, poniewa� platforma jest tworzona
    // w swoich docelowych wymiarach w setupPlatformModel.
    // Je�li jednak chcia�by� dodatkowo skalowa�, mo�esz to tu zrobi�.
    // float platformOverallScale = 1.0f;
    // modelMatrix = glm::scale(modelMatrix, glm::vec3(platformOverallScale));


    shader.setMat4("model", modelMatrix);
    // Kolor platformy (objectColor) i po�yskliwo�� (material_shininess)
    // b�d� ustawiane w main.cpp tu� przed wywo�aniem tej metody Draw.

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount));
    glBindVertexArray(0);
}