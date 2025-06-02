#include "Model.h"
#include <iostream>
#include "stb_image.h"

Model::Model(std::string const& path, bool gamma) : gammaCorrection(gamma)
{
    loadModel(path);
}

void Model::Draw(Shader& shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void Model::loadModel(std::string const& path)
{
    // std::cout << "[Model Load] Attempting to load model: " << path << std::endl;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs | // Ważne dla tekstur ładowanych przez stb_image
        aiProcess_CalcTangentSpace |
        aiProcess_GenSmoothNormals 
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    // std::cout << "[Model Load] SUCCESS: Assimp loaded scene from: " << path << std::endl;
    size_t lastSlash = path.find_last_of("/\\");
    directory = (lastSlash == std::string::npos) ? "." : path.substr(0, lastSlash);
    // std::cout << "[Model Load] Model directory set to: " << directory << std::endl;
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices_data; 
    std::vector<unsigned int> indices_data;
    std::vector<Texture> mesh_textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->HasNormals()) {
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        else {
            vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f); // Domyślna normalna, jeśli brak
        }
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices_data.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices_data.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        mesh_textures.insert(mesh_textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        mesh_textures.insert(mesh_textures.end(), specularMaps.begin(), specularMaps.end());
        
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal"); // Assimp często ładuje normal mapy jako HEIGHT
        mesh_textures.insert(mesh_textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height"); // Lub jako AMBIENT (zależy od eksportera)
        mesh_textures.insert(mesh_textures.end(), heightMaps.begin(), heightMaps.end());
    }
    
    return Mesh(vertices_data, indices_data, mesh_textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> loaded_material_textures;
    unsigned int textureCount = mat->GetTextureCount(type);

    for (unsigned int i = 0; i < textureCount; i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) { 
            if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0) {
                loaded_material_textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture_instance; 
            // Użyj gammaCorrection przy ładowaniu tekstur (szczególnie diffuse)
            bool applyGamma = (typeName == "texture_diffuse") ? this->gammaCorrection : false;
            texture_instance.id = TextureFromFile(str.C_Str(), this->directory, applyGamma);
            if (texture_instance.id != 0) {
                texture_instance.type = typeName;
                texture_instance.path = str.C_Str();
                loaded_material_textures.push_back(texture_instance);
                textures_loaded.push_back(texture_instance);
            }
        }
    }
    return loaded_material_textures;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
    std::string filename_from_model = std::string(path);
    std::string resolved_path = filename_from_model;

    // Poprawione rozwiązywanie ścieżki, aby lepiej radzić sobie ze ścieżkami względnymi i bezwzględnymi
    if (!filename_from_model.empty() && filename_from_model.find(':') == std::string::npos && filename_from_model[0] != '/' && filename_from_model[0] != '\\') {
        // Ścieżka jest względna
        std::string dir_with_slash = directory;
        if (!dir_with_slash.empty() && dir_with_slash.back() != '/' && dir_with_slash.back() != '\\') {
            dir_with_slash += '/';
        }
        resolved_path = dir_with_slash + filename_from_model;
    }
    // std::cout << "[Texture Load] Attempting: \"" << filename_from_model
    //     << "\", Resolved: \"" << resolved_path << "\", Gamma: " << (gamma ? "ON" : "OFF") << std::endl;


    unsigned int textureID = 0;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    // stbi_set_flip_vertically_on_load(true); // Odwracanie UV jest już w aiProcess_FlipUVs
    unsigned char* data = stbi_load(resolved_path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        // std::cout << "[Texture Load] SUCCESS: " << resolved_path << " (W: " << width << ", H: " << height << ", Comp: " << nrComponents << ")" << std::endl;
        GLenum internalFormat = GL_RGB; // Domyślny format wewnętrzny
        GLenum dataFormat = GL_RGB;     // Domyślny format danych
        
        if (nrComponents == 1) { internalFormat = GL_RED; dataFormat = GL_RED; }
        else if (nrComponents == 3) { internalFormat = gamma ? GL_SRGB : GL_RGB; dataFormat = GL_RGB; }
        else if (nrComponents == 4) { internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA; dataFormat = GL_RGBA; }
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else {
        std::cout << "[Texture Load] FAILED: " << resolved_path << std::endl;
        std::cout << "[Texture Load] STB Reason: " << stbi_failure_reason() << std::endl;
        glDeleteTextures(1, &textureID); // Zwolnij ID tekstury, jeśli ładowanie się nie powiodło
        return 0; // Zwróć 0, aby zasygnalizować błąd
    }
    return textureID;
}
