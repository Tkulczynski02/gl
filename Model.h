#pragma once

#include <vector>
#include <string>
// Usun¹³em <map>, bo textures_loaded to vector. Jeœli u¿ywasz mapy gdzie indziej, przywróæ.
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"   // Zawiera teraz poprawn¹ definicjê Vertex i Texture
#include "Shader.h"
#include "stb_image.h" // Potrzebne dla TextureFromFile

class Model
{
public:
    std::vector<Texture> textures_loaded; // Przechowuje wszystkie tekstury za³adowane dla tego modelu
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    Model(std::string const& path, bool gamma = false);
    void Draw(Shader& shader);

private:
    void loadModel(std::string const& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
};