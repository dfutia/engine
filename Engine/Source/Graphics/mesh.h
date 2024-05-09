#pragma once
#ifndef MESH_H
#define MESH_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <spdlog/spdlog.h>

#include <vector>

struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texCoords;
};

struct Mesh {
	~Mesh() {
		spdlog::info("~Mesh()");
	}
	unsigned int vao, vbo, ebo;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

Mesh setupMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

#endif