#pragma once
#ifndef MESH_H
#define MESH_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>

struct Vertex {
	glm::vec3 position;
	glm::vec2 texCoords;
};

struct Mesh {
	unsigned int vao, vbo, ebo;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

Mesh setupMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

#endif