#include "skeleton.h"
#include "util.h"
#include "Animation/animationclip.h"

#include <glm/gtx/string_cast.hpp>
#include <spdlog/spdlog.h>

#include <iostream>

void Skeleton::print() {
    spdlog::info("Bone Count {}", this->boneCount);
}

Skeleton createSkeleton(const aiScene* scene) {
    Skeleton skeleton;

    std::unordered_map<std::string, int> boneNameToIndex;

    // Process all meshes and extract bones
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumBones; ++j) {
            aiBone* ai_bone = mesh->mBones[j];
            std::string boneName(ai_bone->mName.C_Str());

            if (boneNameToIndex.find(boneName) == boneNameToIndex.end()) {
                Bone bone;
                bone.name = boneName;
                bone.invBindPose = convertMatrixToGLMFormat(ai_bone->mOffsetMatrix);
                bone.parentIndex = -1;  // We'll fix this later

                int boneIndex = skeleton.bones.size();
                skeleton.bones.push_back(bone);
                boneNameToIndex[boneName] = boneIndex;
            }
        }
    }

    skeleton.boneCount = skeleton.bones.size();
    skeleton.boneMatrices.resize(skeleton.boneCount, glm::mat4(1.0f));

    // Fix parent indices
    std::function<void(const aiNode*, int)> fixParents = [&](const aiNode* node, int parentIndex) {
        if (boneNameToIndex.find(node->mName.C_Str()) != boneNameToIndex.end()) {
            int boneIndex = boneNameToIndex[node->mName.C_Str()];
            skeleton.bones[boneIndex].parentIndex = parentIndex;
        }
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            fixParents(node->mChildren[i], boneNameToIndex[node->mName.C_Str()]);
        }
        };

    fixParents(scene->mRootNode, -1);

    return skeleton;
}

void updateBoneMatrices(Skeleton& skeleton, const AnimationClip& clip, float timeInSeconds) {
    if (skeleton.boneCount == 0 || clip.channels.empty()) {
        throw std::runtime_error("Invalid skeleton or animation clip data.");
    }

    float ticksPerSecond = clip.ticksPerSecond != 0 ? clip.ticksPerSecond : 25.0f;
    float timeInTicks = timeInSeconds * ticksPerSecond;
    float animationTime = fmod(timeInTicks, clip.duration);

    for (auto& bone : skeleton.bones) {
        bone.localTransform = glm::mat4(1.0f); // Reset local transform

        // Interpolate position
        if (!bone.positions.empty()) {
            glm::vec3 position = interpolatePosition(animationTime, bone);
            bone.localTransform = glm::translate(bone.localTransform, position);
        }

        // Interpolate rotation
        if (!bone.rotations.empty()) {
            glm::quat rotation = interpolateRotation(animationTime, bone);
            bone.localTransform *= glm::mat4_cast(rotation);
        }

        // Interpolate scaling
        if (!bone.scales.empty()) {
            glm::vec3 scale = interpolateScaling(animationTime, bone);
            bone.localTransform = glm::scale(bone.localTransform, scale);
        }

        glm::mat4 globalTransformation = bone.localTransform;
        if (bone.parentIndex >= 0) {
            globalTransformation = skeleton.boneMatrices[bone.parentIndex] * globalTransformation;
        }

        skeleton.boneMatrices[bone.parentIndex] = globalTransformation;
    }

    for (int i = 0; i < skeleton.boneCount; ++i) {
        skeleton.boneMatrices[i] = skeleton.boneMatrices[i] * skeleton.bones[i].invBindPose;
    }
}

float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    float scaleFactor = 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}

glm::vec3 interpolatePosition(float animationTime, const Bone& bone) {
    if (bone.positions.size() == 1) {
        return bone.positions[0].position;
    }

    for (size_t i = 0; i < bone.positions.size() - 1; i++) {
        if (animationTime < bone.positions[i + 1].timeStamp) {
            float scaleFactor = getScaleFactor(bone.positions[i].timeStamp, bone.positions[i + 1].timeStamp, animationTime);
            return glm::mix(bone.positions[i].position, bone.positions[i + 1].position, scaleFactor);
        }
    }

    return bone.positions.back().position;
}

glm::quat interpolateRotation(float animationTime, const Bone& bone) {
    if (bone.rotations.size() == 1) {
        return glm::normalize(bone.rotations[0].orientation);
    }

    for (size_t i = 0; i < bone.rotations.size() - 1; i++) {
        if (animationTime < bone.rotations[i + 1].timeStamp) {
            float scaleFactor = getScaleFactor(bone.rotations[i].timeStamp, bone.rotations[i + 1].timeStamp, animationTime);
            return glm::slerp(bone.rotations[i].orientation, bone.rotations[i + 1].orientation, scaleFactor);
        }
    }

    return glm::normalize(bone.rotations.back().orientation);
}

glm::vec3 interpolateScaling(float animationTime, const Bone& bone) {
    if (bone.scales.size() == 1) {
        return bone.scales[0].scale;
    }

    for (size_t i = 0; i < bone.scales.size() - 1; i++) {
        if (animationTime < bone.scales[i + 1].timeStamp) {
            float scaleFactor = getScaleFactor(bone.scales[i].timeStamp, bone.scales[i + 1].timeStamp, animationTime);
            return glm::mix(bone.scales[i].scale, bone.scales[i + 1].scale, scaleFactor);
        }
    }

    return bone.scales.back().scale;
}