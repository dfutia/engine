#pragma once
#ifndef MESH_H
#define MESH_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <spdlog/spdlog.h>

#include <vector>

struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texCoords;

	// Animation data
	int boneIds[4] = { 0, 0, 0, 0 };
	float boneWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

struct Mesh {
	unsigned int vao, vbo, ebo;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

Mesh setupMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

#endif