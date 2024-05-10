#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "Graphics/mesh.h"
#include "Graphics/texture.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <vector>
#include <string>

struct aiMesh;
struct aiScene;
struct aiNode;

struct Model {
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
	std::string directory;
};

void processNode(aiNode* node, const aiScene* scene, Model& model);
Mesh processMesh(aiMesh* mesh, const aiScene* scene, Model& model);
std::vector<Texture> loadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName, Model& model);

#endif 