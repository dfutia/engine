#pragma once
#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "Graphics/model.h"
#include "Graphics/texture.h"

#include <spdlog/spdlog.h>

#include <map>

struct Assets {
    std::map<std::string, Model> models;
    std::map<std::string, TextureId> textures;
};
extern Assets gAssets;

void addModel(Assets& assets, const std::string& filePath, Model model);
Model getModel(Assets& assets, const std::string& filePath);
void addTexture(Assets& assets, const std::string& filePath, TextureId texture);
TextureId getTexture(Assets& assets, const std::string& filePath);

#endif 
