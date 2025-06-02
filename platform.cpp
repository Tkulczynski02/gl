#include "Platform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector> // Ju¿ do³¹czone w .h, ale dla jasnoœci

// Funkcja pomocnicza do dodawania wierzcho³ków szeœcianu/prostopad³oœcianu
// (pozycja x,y,z, normalna nx,ny,nz) - taka sama jak w Player.cpp
// Ka¿dy wierzcho³ek to 6 floatów: Px, Py, Pz, Nx, Ny, Nz
// Ta funkcja tworzy szeœcian/prostopad³oœcian o œrodku w (0,0,0) lokalnie,
// a potem przesuwamy go macierz¹ modelu w Draw().
// LUB: Mo¿emy przekazaæ wymiary do tej funkcji i ona od razu stworzy prostopad³oœcian o zadanych wymiarach.
// Wybierzmy drug¹ opcjê dla Platform, aby setupPlatformModel by³ prostszy.

// Funkcja pomocnicza do dodawania wierzcho³ków prostopad³oœcianu o zadanych wymiarach,
// wycentrowanego wokó³ lokalnego (0,0,0)
void addCuboidVertices(std::vector<float>& vertices, float width, float height, float depth) {
    float hx = width / 2.0f;
    float hy = height / 2.0f;
    float hz = depth / 2.0f;

    // Pozycje wzglêdem lokalnego œrodka (0,0,0)
    // Przód (+Z) nx=0, ny=0, nz=1
    vertices.insert(vertices.end(), { -hx,-hy, hz, 0,0,1,  hx,-hy, hz, 0,0,1,  hx, hy, hz, 0,0,1,  hx, hy, hz, 0,0,1, -hx, hy, hz, 0,0,1, -hx,-hy, hz, 0,0,1 });
    // Ty³ (-Z) nx=0, ny=0, nz=-1
    vertices.insert(vertices.end(), { -hx,-hy,-hz, 0,0,-1,  hx,-hy,-hz, 0,0,-1,  hx, hy,-hz, 0,0,-1,  hx, hy,-hz, 0,0,-1, -hx, hy,-hz, 0,0,-1, -hx,-hy,-hz, 0,0,-1 });
    // Lewo (-X) nx=-1, ny=0, nz=0
    vertices.insert(vertices.end(), { -hx,-hy,-hz, -1,0,0, -hx,-hy, hz, -1,0,0, -hx, hy, hz, -1,0,0, -hx, hy, hz, -1,0,0, -hx, hy,-hz, -1,0,0, -hx,-hy,-hz, -1,0,0 });
    // Prawo (+X) nx=1, ny=0, nz=0
    vertices.insert(vertices.end(), { hx,-hy,-hz,  1,0,0,  hx,-hy, hz,  1,0,0,  hx, hy, hz,  1,0,0,  hx, hy, hz,  1,0,0,  hx, hy,-hz,  1,0,0,  hx,-hy,-hz,  1,0,0 });
    // Dó³ (-Y) nx=0, ny=-1, nz=0
    vertices.insert(vertices.end(), { -hx,-hy,-hz, 0,-1,0,  hx,-hy,-hz, 0,-1,0,  hx,-hy, hz, 0,-1,0,  hx,-hy, hz, 0,-1,0, -hx,-hy, hz, 0,-1,0, -hx,-hy,-hz, 0,-1,0 });
    // Góra (+Y) nx=0, ny=1, nz=0
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

    // U¿yj wymiarów platformy przekazanych do konstruktora
    addCuboidVertices(platformVertexData, this->size.x, this->size.y, this->size.z);

    vertexCount = platformVertexData.size() / 6; // 6 floatów na wierzcho³ek (3 pozycja + 3 normalna)

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
    // Atrybut wspó³rzêdnych tekstury (location = 2)
    // Nawet jeœli nie u¿ywamy tekstur dla platform (u¿ywamy objectColor),
    // nasz basic.vert mo¿e oczekiwaæ tego atrybutu.
    // Mo¿emy wys³aæ sta³e wartoœci (0,0) jeœli dane nie s¹ w VBO,
    // lub dodaæ TexCoords do addCuboidVertices i tu je ustawiæ.
    // Jeœli basic.vert ma `layout (location = 2) in vec2 aTexCoords;` to musi byæ on w³¹czony.
    // Proste rozwi¹zanie, jeœli Vertex struct ma TexCoords, a my ich nie ustawiamy w addCuboidVertices:
    // dane bêd¹ œmieciowe, ale jeœli shader ich nie u¿ywa, nie ma problemu.
    // Dla shadera z objectColor, TexCoords nie s¹ u¿ywane w basic.frag.
    // Mo¿na by dodaæ dummy TexCoords (0,0) do danych wierzcho³ków (np. 8 floatów na wierzcho³ek)
    // i ustawiæ tu glVertexAttribPointer dla location 2.
    // Na razie zak³adamy, ¿e basic.vert przekazuje aTexCoords, a basic.frag (z objectColor) je ignoruje.


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // std::cout << "[Platform] Model setup. Vertices: " << vertexCount << std::endl;
}


void Platform::Draw(Shader& shader) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // Platforma jest tworzona o swoich rzeczywistych wymiarach wokó³ lokalnego (0,0,0).
    // `this->position` to pozycja jej œrodka w œwiecie gry.
    modelMatrix = glm::translate(modelMatrix, this->position);

    // Rotacja, jeœli potrzebna (np. dla budynków obróconych o 90 stopni)
    float rotationAngleDegrees = 90.0f; // <<<< ZACHOWAJ LUB ZMIEÑ, jeœli chcesz inn¹ orientacjê
    glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngleDegrees), rotationAxis);

    // Skalowanie nie jest ju¿ tutaj potrzebne, poniewa¿ platforma jest tworzona
    // w swoich docelowych wymiarach w setupPlatformModel.
    // Jeœli jednak chcia³byœ dodatkowo skalowaæ, mo¿esz to tu zrobiæ.
    // float platformOverallScale = 1.0f;
    // modelMatrix = glm::scale(modelMatrix, glm::vec3(platformOverallScale));


    shader.setMat4("model", modelMatrix);
    // Kolor platformy (objectColor) i po³yskliwoœæ (material_shininess)
    // bêd¹ ustawiane w main.cpp tu¿ przed wywo³aniem tej metody Draw.

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount));
    glBindVertexArray(0);
}