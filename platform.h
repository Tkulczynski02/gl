#pragma once

#include <glm/glm.hpp>
#include "Shader.h" 
#include <vector>    
#include <GL/glew.h> 

class Platform {
public:
    Platform(glm::vec3 center_position, glm::vec3 dimensions);
    ~Platform();

    void Draw(Shader& shader);
    glm::vec3 getPosition() const { return position; } 
    glm::vec3 getSize() const { return size; }       

private:
    glm::vec3 position; 
    glm::vec3 size;     

    unsigned int VAO, VBO;
    size_t vertexCount;

    void setupPlatformModel(); 
};
