#include "Asset/asset.h"
#include "Core/file.h"

#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <stb_image.h>

#include <iostream>
#include <fstream>
#include <sstream>

Assets gAssets;

void loadGameAssets() {
    auto program = loadShader(gAssets, "Assets/Shaders/skinned.vert", "Assets/Shaders/texture.frag");
    program->use();
    program->setUniformInt("texture1", 0);
    program->setUniformInt("texture2", 1);

    loadModel(gAssets, "Assets/Meshes/Mutant.fbx");
    loadModel(gAssets, "Assets/Meshes/Mutant/Mutant.dae");

    loadTexture(gAssets, "Assets/Textures/container.jpg", "texture_diffuse");
    loadTexture(gAssets, "Assets/Textures/awesomeface.png", "texture_diffuse");
}

size_t generateHash(const std::string& path) {
    std::hash<std::string> hasher;
    return hasher(path);
}

size_t generateHash(const std::string& path1, const std::string& path2) {
    auto hasher = std::hash<std::string>{};
    auto hash1 = hasher(path1);
    auto hash2 = hasher(path2);
    // Simple combination technique: bit shift and exclusive OR
    return hash1 ^ (hash2 << 1);
}

ShaderProgram* loadShader(Assets& assets, const std::string& vertexPath, const std::string& fragmentPath) {
    Handle handle = generateHash(vertexPath, fragmentPath);
    auto it = assets.shaders.find(handle);
    if (it != assets.shaders.end()) {
        return it->second.get(); 
    }

    try {
        // Open files and read into streams
        std::string vertexCode = readFileToString(vertexPath);
        std::string fragmentCode = readFileToString(fragmentPath);

        // Create and compile shaders
        auto vertexShader = std::make_shared<Shader>(vertexCode, GL_VERTEX_SHADER);
        auto fragmentShader = std::make_shared<Shader>(fragmentCode, GL_FRAGMENT_SHADER);

        // Link shaders into a program
        auto program = std::make_unique<ShaderProgram>();
        program->attach(vertexShader);
        program->attach(fragmentShader);
        program->link();

        assets.shaders[handle] = std::move(program);

        return assets.shaders[handle].get();
    }
    catch (const std::exception& e) {
        spdlog::error("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: {}", e.what());
        return nullptr; // Return a nullptr to indicate failure
    }
}

Texture* loadTexture(Assets& assets, const std::string& filePath, const std::string& type) {

    Handle handle = generateHash(filePath);
    auto it = assets.textures.find(handle);
    if (it != assets.textures.end()) {
        spdlog::warn("Texture has already been loaded: {}", filePath);
        return it->second.get();
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Texture Wrapping Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Texture Filtering Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else {
        spdlog::error("Failed to load texture from path: {}", filePath);
        stbi_image_free(data); // ensure memory is freed in case of failure
        glDeleteTextures(1, &textureID); // Delete the texture if loading failed
        return nullptr;
    }

    auto texture = std::make_unique<Texture>();
    texture->id = textureID;
    texture->type = type;

    assets.textures[handle] = std::move(texture);

    spdlog::info("Texture loaded");

    return assets.textures[handle].get();
}

Model* loadModel(Assets& assets, const std::string& filePath) {
    Handle handle = generateHash(filePath);
    auto it = assets.models.find(handle);
    if (it != assets.models.end()) {
        spdlog::info("Model has already been loaded {}", filePath);
        return it->second.get();
    }

    Assimp::Importer importer;
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

    unsigned flags = aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_JoinIdenticalVertices |
        aiProcess_FixInfacingNormals | aiProcess_GenUVCoords | aiProcess_OptimizeMeshes;

    const aiScene* scene = importer.ReadFile(filePath, flags);

    if (scene == nullptr) {
        spdlog::error("Assimp Import Error (null scene): {}", importer.GetErrorString());
        return nullptr;
    }

    if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        std::string const errorString = importer.GetErrorString();

        if (!errorString.empty()) {
            spdlog::error("Assimp Import Error (incomplete scene): {}", importer.GetErrorString());
            return nullptr;
        }
        spdlog::warn("Assimp flagged incomplete scene");
    }

    if (scene->mRootNode == nullptr) {
        spdlog::error("No root node for model, cannot process");
        return nullptr;
    }

    auto model = std::make_unique<Model>();
    model->directory = filePath.substr(0, filePath.find_last_of("/\\"));
    processNode(scene->mRootNode, scene, *model);

    assets.models[handle] = std::move(model);

    spdlog::info("Model loaded");

    return assets.models[handle].get();
}
