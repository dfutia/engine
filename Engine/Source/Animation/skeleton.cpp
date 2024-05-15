#include "skeleton.h"
#include "util.h"
#include "Animation/animationclip.h"

#include <spdlog/spdlog.h>

void Skeleton::print() {
    spdlog::info("Bone Count {}", this->boneCount);
}

Skeleton createSkeleton(const aiScene* scene) {
	Skeleton skeleton;

    // Process all meshes and extract bones
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumBones; j++) {
            aiBone* bone = mesh->mBones[j];
            Bone skeletonBone;
            skeletonBone.name = bone->mName.C_Str();
            skeletonBone.invBindPose = convertMatrixToGLMFormat(bone->mOffsetMatrix);

            // Find or create the parent joint
            int parentIndex = -1;  // If no parent, set to -1
            std::string parentName = skeletonBone.name;  // Modify this to find the actual parent name if available
            auto it = skeleton.boneNameToIndex.find(parentName);
            if (it != skeleton.boneNameToIndex.end()) {
                parentIndex = it->second;
            }

            skeletonBone.parentIndex = parentIndex;
            skeleton.bones.push_back(skeletonBone);
            skeleton.boneNameToIndex[skeletonBone.name] = skeleton.bones.size() - 1;
        }
    }

    skeleton.boneCount = skeleton.bones.size();
    return skeleton;
}

void updateBoneMatrices(Skeleton& skeleton, const AnimationClip& clip, float timeInSeconds) {
    float ticksPerSecond = clip.ticksPerSecond != 0 ? clip.ticksPerSecond : 25.0f;
    float timeInTicks = timeInSeconds * ticksPerSecond;
    float animationTime = fmod(timeInTicks, clip.duration);

    for (const auto& channel : clip.channels) {
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
    if (channel == nullptr || channel->mNumPositionKeys == 0) {
        return glm::vec3(0.0f);
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
    if (channel == nullptr || channel->mNumRotationKeys == 0) {
        return glm::quat();
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
    if (channel == nullptr || channel->mNumScalingKeys == 0) {
        return glm::vec3(1.0f); // Default to no scaling
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