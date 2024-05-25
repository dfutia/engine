#include "animation.h"
#include "util.h"
#include "Graphics/model.h"

#include <assimp/scene.h>

Bone* Animation::findBone(const std::string& name) {
	auto iter = std::find_if(bones.begin(), bones.end(),
		[&](const Bone& Bone)
		{
			return Bone.name == name;
		}
	);
	if (iter == bones.end()) {
		//spdlog::info("bone not found {}", name);
		return nullptr;
	}
	else {
		spdlog::info("bone found {}", name);
		return &(*iter);
	}
}

void Animation::readMissingBones(const aiAnimation* animation, Model& model) {
	int size = animation->mNumChannels;

	auto& boneInfoMap = model.boneInfoMap;
	int& boneCount = model.boneCounter;

	// Reading channels (bones engaged in an animation and their keyframes)
	for (int i = 0; i < size; i++) {
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;
		spdlog::info("Animation Bone Name {}", boneName);

		if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
			spdlog::info("Bone '{}' from animation not found in model.", boneName);
			boneInfoMap[boneName].id = boneCount;
			boneInfoMap[boneName].offsetMatrix = glm::mat4(1.0f); // Initialize with identity matrix or correct offset matrix
			boneCount++;
		}

		bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
	}

	// Update the model's bone information
	model.boneInfoMap = boneInfoMap;
	model.boneCounter = boneCount;
}

void Animation::readHierarchyData(AssimpNodeData& dest, const aiNode* src) {
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = convertMatrixToGLMFormat(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (int i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		readHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}