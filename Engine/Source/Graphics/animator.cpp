#include "animator.h"
#include "animation.h"
#include "bone.h"
#include "animdata.h"
#include "model.h"

#include <iostream>

Animator::Animator(Animation* animation, Model* model)
{
	m_CurrentTime = 0.0;
	m_CurrentAnimation = animation;

	m_FinalBoneMatrices.reserve(200);

	for (int i = 0; i < 200; i++)
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));

	ResolveBoneMappings(animation, model);
}

void Animator::ResolveBoneMappings(Animation* animation, Model* model) {
	auto& boneInfoMap = model->m_BoneInfoMap;
	int& boneCount = model->m_BoneCounter;

	for (int i = 0; i < animation->getBones().size(); i++) {
		Bone& bone = animation->getBones()[i];
		std::string boneName = bone.GetBoneName();

		if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
			std::cerr << "Warning: No matching bone found for " << boneName << "\n";
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}
		else {
			std::cerr << "Mapping bone " << boneName << " to ID " << boneInfoMap[boneName].id << "\n";
		}

		m_BoneMapping[boneName] = boneInfoMap[boneName].id;
	}

	animation->setBoneIDMap(boneInfoMap);
}

void Animator::UpdateAnimation(float dt)
{
	if (m_CurrentAnimation)
	{
		m_CurrentTime += m_CurrentAnimation->getTicksPerSecond() * dt;
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->getDuration());
		CalculateBoneTransform(&m_CurrentAnimation->getRootNode(), glm::mat4(1.0f));
	}
}

void Animator::PlayAnimation(Animation* pAnimation, Model* model)
{
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
	ResolveBoneMappings(pAnimation, model);
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* Bone = m_CurrentAnimation->findBone(nodeName);

	if (Bone)
	{
		Bone->Update(m_CurrentTime);
		nodeTransform = Bone->GetLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = m_CurrentAnimation->getBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		CalculateBoneTransform(&node->children[i], globalTransformation);
}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices()
{
	return m_FinalBoneMatrices;
}
