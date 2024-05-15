#pragma once
#ifndef MESH_H
#define MESH_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <spdlog/spdlog.h>

#include <vector>

const int MAX_BONES_PER_VERTEX = 4;

struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texCoords;

	// Animatin data
	glm::ivec4 boneIndices;
	glm::vec4 boneWeights;
};

struct Mesh {
	unsigned int vao, vbo, ebo;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

Mesh setupMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

#endif