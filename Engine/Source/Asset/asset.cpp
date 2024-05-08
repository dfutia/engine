#include "Asset/asset.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

Assets gAssets;

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

ShaderProgram loadShader(Assets& assets, const std::string& vertexPath, const std::string& fragmentPath) {
    size_t shaderHash = generateCombinedHash(vertexPath, fragmentPath);

    // Check if the shader program is already loaded
    auto it = assets.shaders.find(shaderHash);
    if (it != assets.shaders.end()) {
        //spdlog::info("Shader has already been loaded");
        return it->second; // Return the already loaded shader program
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
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        return ShaderProgram{}; // Return an empty or default shader program
    }

    // Create and compile shaders
    Shader vertexShader(vertexCode.c_str(), GL_VERTEX_SHADER);
    Shader fragmentShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);

    // Link shaders into a program
    ShaderProgram program;
    program.attach(vertexShader);
    program.attach(fragmentShader);
    program.link();

    // Store the new shader program in the cache
    assets.shaders[shaderHash] = program;

    return program;
}

Model loadModel(Assets& assets, const std::string& filePath) {
    size_t fileHash = generateHash(filePath);

    auto it = assets.models.find(fileHash);
    if (it != assets.models.end()) {
        //spdlog::info("Model has already been loaded");
        return it->second; 
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        spdlog::error("ASSIMP: {}", importer.GetErrorString());
        return {};
    }

    Model model;
    processNode(scene->mRootNode, scene, model);

    assets.models[fileHash] = model;

    return model;
}

Texture loadTexture(Assets& assets, const char* filePath) {
    size_t fileHash = generateHash(filePath);

    auto it = assets.textures.find(fileHash);
    if (it != assets.textures.end()) {
        //spdlog::info("Texture has already been loaded");
        return it->second;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Texture Wrapping Parameters (adjust these as needed)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Texture Filtering Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
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
        // Consider setting textureID to a default texture in case of failure
    }

    assets.textures[fileHash] = textureID;

    return textureID;
}