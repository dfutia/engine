#pragma once
#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "Graphics/model.h"
#include "Graphics/texture.h"
#include "Graphics/shader.h"

#include <spdlog/spdlog.h>

#include <functional>
#include <map>
#include <unordered_map>

struct Assets {
    std::unordered_map<size_t, std::shared_ptr<Model>> models;
    std::unordered_map<size_t, Texture> textures;
    std::unordered_map<size_t, std::shared_ptr<ShaderProgram>> shaders;
};

void loadGameAssets();

size_t generateCombinedHash(const std::string& path1, const std::string& path2);
size_t generateHash(const std::string& key);
std::string loadShaderSource(const std::string& filepath);

std::shared_ptr<ShaderProgram> loadShader(Assets& assets, const std::string& vertexPath, const std::string& fragmentPath);
std::shared_ptr<Model> loadModel(Assets& assets, const std::string& filePath);
Texture loadTexture(Assets& assets, const char* filePath);

extern Assets gAssets;

#endif 
