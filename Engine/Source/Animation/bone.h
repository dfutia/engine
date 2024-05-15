#pragma once
#ifndef BONE_H
#define BONE_H

#include <util.h>

#include <assimp/scene.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <vector>

struct KeyPosition {
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation {
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale {
    glm::vec3 scale;
    float timeStamp;
};

struct Bone {
    std::string name;
    int parentIndex;
    glm::mat4 invBindPose;
    glm::mat4 localTransform;
    std::vector<KeyPosition> positions;
    std::vector<KeyRotation> rotations;
    std::vector<KeyScale> scales;

    Bone() : parentIndex(-1), localTransform(1.0f) {}

    void initialize(const std::string& boneName, int id, const aiNodeAnim* channel) {
        name = boneName;
        parentIndex = id;
        localTransform = glm::mat4(1.0f);

        if (channel) {
            // Load positions
            for (unsigned int i = 0; i < channel->mNumPositionKeys; ++i) {
                KeyPosition key;
                key.position = getGLMVec(channel->mPositionKeys[i].mValue);
                key.timeStamp = channel->mPositionKeys[i].mTime;
                positions.push_back(key);
            }

            // Load rotations
            for (unsigned int i = 0; i < channel->mNumRotationKeys; ++i) {
                KeyRotation key;
                key.orientation = getGLMQuat(channel->mRotationKeys[i].mValue);
                key.timeStamp = channel->mRotationKeys[i].mTime;
                rotations.push_back(key);
            }

            // Load scales
            for (unsigned int i = 0; i < channel->mNumScalingKeys; ++i) {
                KeyScale key;
                key.scale = getGLMVec(channel->mScalingKeys[i].mValue);
                key.timeStamp = channel->mScalingKeys[i].mTime;
                scales.push_back(key);
            }
        }
    }
};


#endif 