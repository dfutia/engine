#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "Graphics/mesh.h"

#include <vector>

struct aiMesh;
struct aiScene;
struct aiNode;

struct Model {
	std::vector<Mesh> meshes;
};

void processNode(aiNode* node, const aiScene* scene, Model& model);
Mesh processMesh(aiMesh* mesh, const aiScene* scene);

#endif 