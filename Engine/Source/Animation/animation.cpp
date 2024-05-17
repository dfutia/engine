#include "animation.h"
#include "bone.h"
#include "boneinfo.h"

#include <spdlog/spdlog.h>

#include <algorithm>

// https://github.com/grishavanika/skeletal_animation_sample/blob/eb663488d6a7a32bce0f00f41d58965e5d4a9379/main.cpp#L1094

//std::string normalizeBoneName(const std::string& boneName) {
//    std::string normalized = boneName;
//    std::replace(normalized.begin(), normalized.end(), ':', '_');
//    return normalized;
//}

Animation::Animation(const aiScene* scene, const std::map<std::string, BoneInfo>& boneInfoMap) {
    if (scene->mNumAnimations == 0) {
        spdlog::error("No animations found in the scene");
        return;
    }

    aiAnimation* animation = scene->mAnimations[0];

    duration = animation->mDuration;
    ticksPerSecond = animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f;

    // Iterate over animation channels and extract bone information
    for (unsigned int i = 0; i < animation->mNumChannels; ++i) {
        aiNodeAnim* channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.C_Str();
        auto it = boneInfoMap.find(boneName);
        if (it != boneInfoMap.end()) {
            int boneId = it->second.id;
            bones.emplace_back(boneName, boneId, channel);
            spdlog::info("Bone {} found in animation with ID {}", boneName, boneId);
        }
        else {
            spdlog::warn("Bone not found in boneInfoMap: {}", boneName);
        }
    }

    // Build the bone hierarchy
    for (const auto& boneInfo : boneInfoMap) {
        const std::string& boneName = boneInfo.first;
        int boneId = boneInfo.second.id;

        const aiNode* node = scene->mRootNode->FindNode(boneName.c_str());
        if (node) {
            if (node->mParent) {
                std::string parentName = node->mParent->mName.C_Str();
                auto parentIt = boneInfoMap.find(parentName);
                if (parentIt != boneInfoMap.end()) {
                    int parentId = parentIt->second.id;
                    boneHierarchy[parentId].push_back(boneId);
                }
            }
        }
    }
}