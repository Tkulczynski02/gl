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
    std::cout << "[Model Load] Attempting to load model: " << path << std::endl;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace |
        aiProcess_GenSmoothNormals
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    std::cout << "[Model Load] SUCCESS: Assimp loaded scene from: " << path << std::endl;
    size_t lastSlash = path.find_last_of("/\\");
    directory = (lastSlash == std::string::npos) ? "." : path.substr(0, lastSlash);
    std::cout << "[Model Load] Model directory set to: " << directory << std::endl;
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
    std::vector<Vertex> vertices_data; // Zmieniono nazwê, aby nie kolidowaæ z polem klasy Mesh
    std::vector<unsigned int> indices_data;
    std::vector<Texture> mesh_textures;

    // std::cout << "[Mesh Process] Processing mesh: " << (mesh->mName.C_Str() ? mesh->mName.C_Str() : "Unnamed") 
    //           << " with " << mesh->mNumVertices << " vertices." << std::endl;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->HasNormals()) {
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        else {
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
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
        // std::cout << "[Mesh Process] Material index: " << mesh->mMaterialIndex << ". Processing material." << std::endl;

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        mesh_textures.insert(mesh_textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        mesh_textures.insert(mesh_textures.end(), specularMaps.begin(), specularMaps.end());
    }
    else {
        // std::cout << "[Mesh Process] WARNING: Mesh " << (mesh->mName.C_Str() ? mesh->mName.C_Str() : "Unnamed") 
        //           << " has no material assigned." << std::endl;
    }

    // Przekazujemy zebrane tekstury (nawet jeœli puste) do konstruktora Mesh
    // Jeœli Twój budynek1.fbx nie ma tekstur, mesh_textures bêdzie puste i Mesh::Draw sobie z tym poradzi.
    return Mesh(vertices_data, indices_data, mesh_textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> loaded_material_textures; // Zmieniono nazwê
    // std::cout << "[Material Textures] Checking material for texture type: \"" << typeName 
    //           << "\" (Assimp aiTextureType ID: " << type << ")" << std::endl;

    unsigned int textureCount = mat->GetTextureCount(type);
    // std::cout << "[Material Textures] Count for this type in material: " << textureCount << std::endl;

    for (unsigned int i = 0; i < textureCount; i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        // std::cout << "[Material Textures] Found texture path in FBX material: \"" << str.C_Str() << "\"" << std::endl;

        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) { // textures_loaded to sk³adowa klasy Model
            if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0) {
                loaded_material_textures.push_back(textures_loaded[j]);
                // std::cout << "[Material Textures] SKIPPED (already loaded globally): \"" << str.C_Str() << "\"" << std::endl;
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture_instance; // Zmieniono nazwê
            texture_instance.id = TextureFromFile(str.C_Str(), this->directory, this->gammaCorrection);
            if (texture_instance.id != 0) {
                texture_instance.type = typeName;
                texture_instance.path = str.C_Str();
                loaded_material_textures.push_back(texture_instance);
                textures_loaded.push_back(texture_instance);
                // std::cout << "[Material Textures] Successfully processed and added texture: \"" << str.C_Str() << "\"" << std::endl;
            }
            else {
                // std::cout << "[Material Textures] WARNING: Texture \"" << str.C_Str() 
                //           << "\" from material failed to load, not adding." << std::endl;
            }
        }
    }
    // if (textureCount == 0) {
    //     std::cout << "[Material Textures] No textures of type \"" << typeName << "\" found in material." << std::endl;
    // }
    // std::cout << "[Material Textures] Finished for type: \"" << typeName << "\". Textures for this mesh: " << loaded_material_textures.size() << std::endl;
    return loaded_material_textures;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
    std::string filename_from_model = std::string(path);
    std::string resolved_path = filename_from_model;

    if (!filename_from_model.empty() &&
        filename_from_model.find(':') == std::string::npos &&
        filename_from_model[0] != '/' && filename_from_model[0] != '\\') {
        std::string dir_with_slash = directory;
        if (!dir_with_slash.empty() && dir_with_slash.back() != '/' && dir_with_slash.back() != '\\') {
            dir_with_slash += '/';
        }
        resolved_path = dir_with_slash + filename_from_model;
    }

    std::cout << "[Texture Load] Attempting: \"" << filename_from_model
        << "\", Resolved: \"" << resolved_path << "\"" << std::endl;

    unsigned int textureID = 0;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(resolved_path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        // std::cout << "[Texture Load] SUCCESS: " << resolved_path << " (W: " << width << ", H: " << height << ", Comp: " << nrComponents << ")" << std::endl;
        GLenum internalFormat = GL_RGB;
        GLenum dataFormat = GL_RGB;
        if (nrComponents == 1) { internalFormat = GL_RED; dataFormat = GL_RED; }
        else if (nrComponents == 3) { internalFormat = gamma ? GL_SRGB : GL_RGB; dataFormat = GL_RGB; }
        else if (nrComponents == 4) { internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA; dataFormat = GL_RGBA; }
        // else { std::cout << "[Texture Load] WARN: Unsupported #comp: " << nrComponents << " for " << resolved_path << std::endl; }

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
        glDeleteTextures(1, &textureID);
        return 0;
    }
    return textureID;
}