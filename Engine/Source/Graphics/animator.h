#pragma once
#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <glm/mat4x4.hpp>

#include <vector>

class Animation;
struct AssimpNodeData;

class Animator {
public:
	std::vector<glm::mat4> finalBoneMatrices;
	Animation* currentAnimation;
	float currentTime;
	float deltaTime;

	Animator();
	Animator(Animation* animation);

	void updateAnimation(float deltaTime);
	void playAnimation(Animation* animation);
	void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
};

#endif 