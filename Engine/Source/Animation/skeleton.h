#pragma once
#ifndef SKELETON_H
#define SKELETON_H

#include "Animation/bone.h"

#include <assimp/scene.h>
#include <glm/mat4x4.hpp>
#include<glm/gtc/quaternion.hpp>

#include <vector>
#include <map>

struct AnimationClip;

struct Skeleton {
	int boneCount;
	std::vector<Bone> bones;
	std::map<std::string, int> boneNameToIndex;
	std::vector<glm::mat4> boneMatrices;

	void print();
};

Skeleton createSkeleton(const aiScene* scene);

void updateBoneMatrices(Skeleton& skeleton, const AnimationClip& clip, float timeInSeconds);

float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
glm::vec3 interpolatePosition(float animationTime, const Bone& bone);
glm::quat interpolateRotation(float animationTime, const Bone& bone);
glm::vec3 interpolateScaling(float animationTime, const Bone& bone);

#endif 