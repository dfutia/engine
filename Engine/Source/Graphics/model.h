#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "Graphics/mesh.h"
#include "Graphics/texture.h"
#include "Animation/boneinfo.h"
#include "Animation/animation.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <string>
#include <map>

struct aiMesh;
struct aiScene;
struct aiNode;

struct Assets;

struct Model {
	std::string directory;
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
	 
	std::map<std::string, BoneInfo> boneInfoMap;
	int boneCounter;
	std::shared_ptr<Animation> currentAnimation;
	std::vector<glm::mat4> boneTransforms;

	void playAnimation(const std::string& animationPath, Assets& assets);
	void updateBoneTransform(float timeInSeconds);
};

void processNode(aiNode* node, const aiScene* scene, Model& model);
Mesh processMesh(aiMesh* mesh, const aiScene* scene, Model& model);
std::vector<Texture> loadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName, Model& model);
void setVertexBoneData(Vertex& vertex, int boneID, float weight);
void extractBoneWeightForVertices(Model& model, std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

#endif 