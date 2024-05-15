#include "skeleton.h"
#include "util.h"
#include "Animation/animationclip.h"

#include <spdlog/spdlog.h>

#include <iostream>

void Skeleton::print() {
    spdlog::info("Bone Count {}", this->boneCount);
}

Skeleton createSkeleton(const aiScene* scene) {
	Skeleton skeleton;

    skeleton.boneCount = scene->mNumMeshes; // or some appropriate count
    skeleton.bones.resize(skeleton.boneCount);
    skeleton.boneMatrices.resize(skeleton.boneCount);

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumBones; ++j) {
            aiBone* ai_bone = mesh->mBones[j];
            std::string boneName(ai_bone->mName.C_Str());

            if (skeleton.boneNameToIndex.find(boneName) == skeleton.boneNameToIndex.end()) {
                int boneIndex = skeleton.bones.size();
                skeleton.boneNameToIndex[boneName] = boneIndex;
                skeleton.bones.emplace_back(Bone(boneName, boneIndex, nullptr)); // Initialize without animation channel

                Bone& bone = skeleton.bones.back();
                bone.invBindPose = ai_bone->mOffsetMatrix;
                std::cout << "Skeleton Bone Name: " << bone.name << std::endl;
            }
        }
    }

    for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
        const aiAnimation* animation = scene->mAnimations[i];
        for (unsigned int j = 0; j < animation->mNumChannels; ++j) {
            const aiNodeAnim* channel = animation->mChannels[j];
            std::string boneName(channel->mNodeName.C_Str());

            if (skeleton.boneNameToIndex.find(boneName) != skeleton.boneNameToIndex.end()) {
                int boneIndex = skeleton.boneNameToIndex[boneName];
                skeleton.bones[boneIndex] = Bone(boneName, boneIndex, channel);
            }
        }
    }

    return skeleton;
}

void updateBoneMatrices(Skeleton& skeleton, const AnimationClip& clip, float timeInSeconds) {
    if (skeleton.boneCount == 0 || clip.channels.empty()) {
        throw std::runtime_error("Invalid skeleton or animation clip data.");
    }

    float ticksPerSecond = clip.ticksPerSecond != 0 ? clip.ticksPerSecond : 25.0f;
    float timeInTicks = timeInSeconds * ticksPerSecond;
    float animationTime = fmod(timeInTicks, clip.duration);

    std::cout << "Updating bone matrices at time: " << animationTime << " seconds" << std::endl;

    for (const auto& channel : clip.channels) {
        std::cout << "Processing channel for node: " << channel->mNodeName.C_Str() << std::endl;

        if (skeleton.boneNameToIndex.find(channel->mNodeName.C_Str()) == skeleton.boneNameToIndex.end()) {
            throw std::runtime_error("Bone name not found in skeleton: " + std::string(channel->mNodeName.C_Str()));
        }

        int jointIndex = skeleton.boneNameToIndex[channel->mNodeName.C_Str()];
        glm::mat4 nodeTransformation = glm::mat4(1.0f); // Identity matrix

        // Interpolate position
        if (channel->mNumPositionKeys > 0) {
            glm::vec3 position = interpolatePosition(animationTime, channel);
            nodeTransformation = glm::translate(nodeTransformation, position);
        }

        // Interpolate rotation
        if (channel->mNumRotationKeys > 0) {
            glm::quat rotation = interpolateRotation(animationTime, channel);
            nodeTransformation *= glm::mat4_cast(rotation);
        }

        // Interpolate scaling
        if (channel->mNumScalingKeys > 0) {
            glm::vec3 scaling = interpolateScaling(animationTime, channel);
            nodeTransformation = glm::scale(nodeTransformation, scaling);
        }

        glm::mat4 globalTransformation = nodeTransformation;
        if (skeleton.bones[jointIndex].parentIndex >= 0) {
            globalTransformation = skeleton.boneMatrices[skeleton.bones[jointIndex].parentIndex] * globalTransformation;
        }

        skeleton.boneMatrices[jointIndex] = globalTransformation;
    }

    for (int i = 0; i < skeleton.boneCount; ++i) {
        skeleton.boneMatrices[i] = skeleton.boneMatrices[i] * skeleton.bones[i].invBindPose;
    }
}

glm::vec3 interpolatePosition(float animationTime, const aiNodeAnim* channel) {
    if (channel == nullptr) {
        throw std::runtime_error("Channel is null.");
    }
    if (channel->mNumPositionKeys == 0) {
        throw std::runtime_error("No position keys in the channel.");
    }

    if (channel->mNumPositionKeys == 1) {
        return getGLMVec(channel->mPositionKeys[0].mValue);
    }

    for (unsigned int i = 0; i < channel->mNumPositionKeys - 1; i++) {
        if (animationTime < channel->mPositionKeys[i + 1].mTime) {
            float deltaTime = channel->mPositionKeys[i + 1].mTime - channel->mPositionKeys[i].mTime;
            float factor = (animationTime - channel->mPositionKeys[i].mTime) / deltaTime;
            const aiVector3D& start = channel->mPositionKeys[i].mValue;
            const aiVector3D& end = channel->mPositionKeys[i + 1].mValue;
            aiVector3D delta = end - start;
            return getGLMVec(start + factor * delta);
        }
    }

    return getGLMVec(channel->mPositionKeys[channel->mNumPositionKeys - 1].mValue);
}

glm::quat interpolateRotation(float animationTime, const aiNodeAnim* channel) {
    if (channel == nullptr) {
        throw std::runtime_error("Channel is null.");
    }
    if (channel->mNumRotationKeys == 0) {
        throw std::runtime_error("No rotation keys in the channel.");
    }

    if (channel->mNumRotationKeys == 1) {
        return getGLMQuat(channel->mRotationKeys[0].mValue);
    }

    for (unsigned int i = 0; i < channel->mNumRotationKeys - 1; i++) {
        if (animationTime < channel->mRotationKeys[i + 1].mTime) {
            float deltaTime = channel->mRotationKeys[i + 1].mTime - channel->mRotationKeys[i].mTime;
            float factor = (animationTime - channel->mRotationKeys[i].mTime) / deltaTime;
            const aiQuaternion& start = channel->mRotationKeys[i].mValue;
            const aiQuaternion& end = channel->mRotationKeys[i + 1].mValue;
            aiQuaternion result;
            aiQuaternion::Interpolate(result, start, end, factor);
            result.Normalize();
            return getGLMQuat(result);
        }
    }

    return getGLMQuat(channel->mRotationKeys[channel->mNumRotationKeys - 1].mValue);
}

glm::vec3 interpolateScaling(float animationTime, const aiNodeAnim* channel) {
    if (channel == nullptr) {
        throw std::runtime_error("Channel is null.");
    }
    if (channel->mNumScalingKeys == 0) {
        throw std::runtime_error("No scaling keys in the channel.");
    }

    if (channel->mNumScalingKeys == 1) {
        return getGLMVec(channel->mScalingKeys[0].mValue);
    }

    for (unsigned int i = 0; i < channel->mNumScalingKeys - 1; i++) {
        if (animationTime < channel->mScalingKeys[i + 1].mTime) {
            float deltaTime = channel->mScalingKeys[i + 1].mTime - channel->mScalingKeys[i].mTime;
            float factor = (animationTime - channel->mScalingKeys[i].mTime) / deltaTime;
            const aiVector3D& start = channel->mScalingKeys[i].mValue;
            const aiVector3D& end = channel->mScalingKeys[i + 1].mValue;
            aiVector3D delta = end - start;
            return getGLMVec(start + factor * delta);
        }
    }

    return getGLMVec(channel->mScalingKeys[channel->mNumScalingKeys - 1].mValue);
}