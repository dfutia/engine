#pragma once
#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "Graphics/model.h"
#include "Graphics/texture.h"
#include "Graphics/shader.h"
#include "Graphics/animation.h"

#include <spdlog/spdlog.h>

#include <functional>
#include <map>
#include <unordered_map>

using Handle = size_t;

struct Assets {
    std::unordered_map<Handle, std::unique_ptr<Texture>> textures;
    std::unordered_map<Handle, std::unique_ptr<Model>> models;
    std::unordered_map<Handle, std::unique_ptr<Animation>> animations;
    std::unordered_map<Handle, std::unique_ptr<ShaderProgram>> shaders;
};

void loadGameAssets();

Handle generateHash(const std::string& path);
Handle generateHash(const std::string& path1, const std::string& path2);

ShaderProgram* loadShader(Assets& assets, const std::string& vertexPath, const std::string& fragmentPath);
Texture* loadTexture(Assets& assets, const std::string& filePath, const std::string& type);
Model* loadModel(Assets& assets, const std::string& filePath);
Animation* loadAnimation(Assets& assets, const std::string& filePath);

extern Assets gAssets;

#endif 