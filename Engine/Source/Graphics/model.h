#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "Graphics/mesh.h"
#include "Graphics/texture.h"
#include "Graphics/boneinfo.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <string>
#include <map>
#include <unordered_map>

struct Assets;

struct aiMesh;
struct aiScene;
struct aiNode;

struct Model {
	std::string directory;
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
	std::unordered_map<std::string, BoneInfo> boneInfoMap;
	int boneCounter = 0;
};

void processNode(aiNode* node, const aiScene* scene, Model& model);
Mesh processMesh(aiMesh* mesh, const aiScene* scene, Model& model);
std::vector<Texture> loadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName, Model& model);
void setVertexBoneDataToDefault(Vertex& vertex);
void setVertexBoneData(Vertex& vertex, int boneID, float weight);
void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene, Model& model);

#endif 