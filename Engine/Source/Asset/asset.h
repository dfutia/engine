#pragma once
#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "Graphics/model.h"
#include "Graphics/texture.h"
#include "Graphics/shader.h"
#include "Animation/animationclip.h"

#include <spdlog/spdlog.h>

#include <functional>
#include <map>
#include <unordered_map>

using Handle = size_t;

struct Assets {
    std::unordered_map<Handle, std::shared_ptr<Texture>> textures;
    std::unordered_map<Handle, std::shared_ptr<Model>> models;
    std::unordered_map<Handle, std::shared_ptr<ShaderProgram>> shaders;
    std::unordered_map < Handle, std::shared_ptr<AnimationClip>> animations;
};

void loadGameAssets();

Handle generateCombinedHash(const std::string& path1, const std::string& path2);
Handle generateHash(const std::string& key);
std::string loadShaderSource(const std::string& filepath);

std::shared_ptr<ShaderProgram> loadShader(Assets& assets, const std::string& vertexPath, const std::string& fragmentPath);
std::shared_ptr<Model> loadModel(Assets& assets, const std::string& filePath);
std::shared_ptr<Texture> loadTexture(Assets& assets, const std::string& filePath, const std::string& type);
std::shared_ptr<AnimationClip> loadAnimation(Assets& assets, const std::string& filePath);

extern Assets gAssets;

#endif 
