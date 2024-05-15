#pragma once
#ifndef BONE_H
#define BONE_H

#include <glm/mat4x4.hpp>

#include <string>

struct Bone {
	std::string name;
	int parentIndex;
	glm::mat4 invBindPose;
};

#endif 