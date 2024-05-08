#pragma once
#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "Graphics/model.h"
#include "Graphics/texture.h"
#include "Graphics/shader.h"

#include <spdlog/spdlog.h>

#include <map>

struct Assets {
    std::map<size_t, Model> models;
    std::map<size_t, Texture> textures;
    std::map<size_t, ShaderProgram> shaders;
};

size_t generateCombinedHash(const std::string& path1, const std::string& path2);
size_t generateHash(const std::string& key);
std::string loadShaderSource(const std::string& filepath);

ShaderProgram  loadShader(Assets& assets, const std::string& vertexPath, const std::string& fragmentPath);
Model loadModel(Assets& assets, const std::string& filePath);
Texture loadTexture(Assets& assets, const char* filePath);

extern Assets gAssets;

#endif 
