#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include <glm/mat4x4.hpp>
#include <assimp/scene.h>
#include "animdata.h"
#include "bone.h"

#include <string>
#include <vector>
#include <map>

class Bone;
struct Model;
struct BoneInfo;

struct AssimpNodeData {
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation {
public:
	Animation() = default;
	Animation(const std::string& animationPath, Model* model);

	Bone* findBone(const std::string& name);

	inline float getTicksPerSecond() { return m_TicksPerSecond; }
	inline float getDuration() { return m_Duration; }
	inline const AssimpNodeData& getRootNode() { return m_RootNode; }
	inline const std::map<std::string, BoneInfo>& getBoneIDMap() {  return m_BoneInfoMap; }
private:
	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;

	void readMissingBones(const aiAnimation* animation, Model& model);
	void readHierarchyData(AssimpNodeData& dest, const aiNode* src);
};

#endif 
