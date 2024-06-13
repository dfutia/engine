#pragma once
#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <glm/glm.hpp>

#include <vector>
#include <map>
#include <string>

struct Model;
class Animation;
struct AssimpNodeData;

class Animator
{
public:
	Animator(Animation* animation, Model* model);

	void ResolveBoneMappings(Animation* animation, Model* model);

	void UpdateAnimation(float dt);

	void PlayAnimation(Animation* pAnimation, Model* model);

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	std::vector<glm::mat4> GetFinalBoneMatrices();
private:
	Model* model;
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	std::map<std::string, int> m_BoneMapping;
	float m_CurrentTime;
};

#endif 