#include "Asset/asset.h"

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
    //auto program = loadShader(gAssets, "Assets/Shaders/texture.vert", "Assets/Shaders/texture.frag");
    //program->use();
    //program->setUniformInt("texture1", 0);
    //program->setUniformInt("texture2", 1);

    auto program = loadShader(gAssets, "Assets/Shaders/skinned.vert", "Assets/Shaders/texture.frag");
    program->use();
    program->setUniformInt("texture1", 0);
    program->setUniformInt("texture2", 1);

    loadModel(gAssets, "Assets/Meshes/Vampire/dancing_vampire.dae");
    loadModel(gAssets, "Assets/Meshes/Maria/Maria J J Ong.dae");
    loadModel(gAssets, "Assets/Meshes/Maria J J Ong.fbx");

    loadTexture(gAssets, "Assets/Textures/container.jpg", "texture_diffuse");
    loadTexture(gAssets, "Assets/Textures/awesomeface.png", "texture_diffuse");
}

std::string loadShaderSource(const std::string& filepath) {
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

size_t generateCombinedHash(const std::string& path1, const std::string& path2) {
    auto hasher = std::hash<std::string>{};
    auto hash1 = hasher(path1);
    auto hash2 = hasher(path2);
    // Simple combination technique: bit shift and exclusive OR
    return hash1 ^ (hash2 << 1);
}

size_t generateHash(const std::string& key) {
    std::hash<std::string> hasher;
    return hasher(key);
}

std::shared_ptr<ShaderProgram> loadShader(Assets& assets, const std::string& vertexPath, const std::string& fragmentPath) {
    Handle handle = generateCombinedHash(vertexPath, fragmentPath);

    auto it = assets.shaders.find(handle);
    if (it != assets.shaders.end()) {
        return it->second; 
    }

    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // Open files and read into streams
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        // Create and compile shaders
        auto vertexShader = std::make_shared<Shader>(vertexCode, GL_VERTEX_SHADER);
        auto fragmentShader = std::make_shared<Shader>(fragmentCode, GL_FRAGMENT_SHADER);

        // Link shaders into a program
        auto program = std::make_shared<ShaderProgram>();
        program->attach(vertexShader);
        program->attach(fragmentShader);
        program->link();

        assets.shaders[handle] = program;
        return program;
    }
    catch (const std::exception& e) {
        //spdlog::error("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: {}", e.what());
        return nullptr; // Return a nullptr to indicate failure
    }
}

std::shared_ptr<Texture> loadTexture(Assets& assets, const std::string& filePath, const std::string& type) {
    Handle handle = generateHash(filePath);

    auto it = assets.textures.find(handle);
    if (it != assets.textures.end()) {
        spdlog::warn("Texture has already been loaded: {}", filePath);
        return it->second;
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

    auto texture = std::make_shared<Texture>();
    texture->id = textureID;
    texture->type = type;
    assets.textures[handle] = texture;

    spdlog::info("Texture loaded");

    return texture;
}

std::shared_ptr<Model> loadModel(Assets& assets, const std::string& filePath) {
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

    //==============================================================
    Handle handle = generateHash(filePath);
    auto it = assets.models.find(handle);
    if (it != assets.models.end()) {
        spdlog::info("Model has already been loaded {}", filePath);
        return it->second;
    }
    //==============================================================

    auto newModel = std::make_shared<Model>();
    newModel->directory = filePath.substr(0, filePath.find_last_of("/\\"));

    processNode(scene->mRootNode, scene, *newModel);

    if (scene->mNumAnimations == 0) {
        spdlog::info("Model has no animations");
    }
    else {
        spdlog::info("Model has animations");
    }

    assets.models[handle] = newModel;

    spdlog::info("Model loaded");

    return newModel;
}