#include "bone.h"
#include "util.h"

#include <spdlog/spdlog.h>

Bone::Bone(const std::string& name, int id, const aiNodeAnim* channel) : name(name), id(id) {
    // Extract position keys
    for (unsigned int i = 0; i < channel->mNumPositionKeys; ++i) {
        KeyPosition data;
        data.position = glm::vec3(channel->mPositionKeys[i].mValue.x, channel->mPositionKeys[i].mValue.y, channel->mPositionKeys[i].mValue.z);
        data.timeStamp = channel->mPositionKeys[i].mTime;
        positions.push_back(data);
    }

    // Extract rotation keys
    for (unsigned int i = 0; i < channel->mNumRotationKeys; ++i) {
        KeyRotation data;
        data.orientation = glm::quat(channel->mRotationKeys[i].mValue.w, channel->mRotationKeys[i].mValue.x, channel->mRotationKeys[i].mValue.y, channel->mRotationKeys[i].mValue.z);
        data.timeStamp = channel->mRotationKeys[i].mTime;
        rotations.push_back(data);
    }

    // Extract scale keys
    for (unsigned int i = 0; i < channel->mNumScalingKeys; ++i) {
        KeyScale data;
        data.scale = glm::vec3(channel->mScalingKeys[i].mValue.x, channel->mScalingKeys[i].mValue.y, channel->mScalingKeys[i].mValue.z);
        data.timeStamp = channel->mScalingKeys[i].mTime;
        scales.push_back(data);
    }
}

glm::vec3 interpolatePosition(float animationTime, const Bone& bone) {
    if (bone.positions.size() == 1) {
        spdlog::info("Single position key for bone {}: [{}, {}, {}]", bone.name, bone.positions[0].position.x, bone.positions[0].position.y, bone.positions[0].position.z);
        return bone.positions[0].position;
    }

    for (size_t i = 0; i < bone.positions.size() - 1; ++i) {
        if (animationTime < bone.positions[i + 1].timeStamp) {
            float t = (animationTime - bone.positions[i].timeStamp) / (bone.positions[i + 1].timeStamp - bone.positions[i].timeStamp);
            glm::vec3 interpolated = glm::mix(bone.positions[i].position, bone.positions[i + 1].position, t);
            spdlog::info("Interpolated Position for bone {}: [{}, {}, {}]", bone.name, interpolated.x, interpolated.y, interpolated.z);
            return interpolated;
        }
    }

    return bone.positions.back().position;
}

glm::quat interpolateRotation(float animationTime, const Bone& bone) {
    if (bone.rotations.size() == 1) {
        spdlog::info("Single rotation key for bone {}: [{}, {}, {}, {}]", bone.name, bone.rotations[0].orientation.x, bone.rotations[0].orientation.y, bone.rotations[0].orientation.z, bone.rotations[0].orientation.w);
        return bone.rotations[0].orientation;
    }

    for (size_t i = 0; i < bone.rotations.size() - 1; ++i) {
        if (animationTime < bone.rotations[i + 1].timeStamp) {
            float t = (animationTime - bone.rotations[i].timeStamp) / (bone.rotations[i + 1].timeStamp - bone.rotations[i].timeStamp);
            glm::quat interpolated = glm::slerp(bone.rotations[i].orientation, bone.rotations[i + 1].orientation, t);
            spdlog::info("Interpolated Rotation for bone {}: [{}, {}, {}, {}]", bone.name, interpolated.x, interpolated.y, interpolated.z, interpolated.w);
            return interpolated;
        }
    }

    return bone.rotations.back().orientation;
}

glm::vec3 interpolateScale(float animationTime, const Bone& bone) {
    if (bone.scales.size() == 1) {
        spdlog::info("Single scale key for bone {}: [{}, {}, {}]", bone.name, bone.scales[0].scale.x, bone.scales[0].scale.y, bone.scales[0].scale.z);
        return bone.scales[0].scale;
    }

    for (size_t i = 0; i < bone.scales.size() - 1; ++i) {
        if (animationTime < bone.scales[i + 1].timeStamp) {
            float t = (animationTime - bone.scales[i].timeStamp) / (bone.scales[i + 1].timeStamp - bone.scales[i].timeStamp);
            glm::vec3 interpolated = glm::mix(bone.scales[i].scale, bone.scales[i + 1].scale, t);
            spdlog::info("Interpolated Scale for bone {}: [{}, {}, {}]", bone.name, interpolated.x, interpolated.y, interpolated.z);
            return interpolated;
        }
    }

    return bone.scales.back().scale;
}