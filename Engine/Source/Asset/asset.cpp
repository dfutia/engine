#include "Asset/asset.h"

Assets gAssets;

void addModel(Assets& assets, const std::string& filePath, Model model) {
	assets.models[filePath] = model;
}

Model getModel(Assets& assets, const std::string& filePath) {
	return assets.models[filePath];
}

void addTexture(Assets& assets, const std::string& filePath, TextureId texture) {
	assets.textures[filePath] = texture;
}

TextureId getTexture(Assets& assets, const std::string& filePath) {
	return assets.textures[filePath];
}