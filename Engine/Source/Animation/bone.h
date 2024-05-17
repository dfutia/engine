#pragma once
#ifndef BONE_H
#define BONE_H

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/scene.h>

#include <string>
#include <vector>

struct KeyPosition {
    glm::vec3 position;
    double timeStamp;
};

struct KeyRotation {
    glm::quat orientation;
    double timeStamp;
};

struct KeyScale {
    glm::vec3 scale;
    double timeStamp;
};

struct Bone {
    std::string name;
    int id;
    std::vector<KeyPosition> positions;
    std::vector<KeyRotation> rotations;
    std::vector<KeyScale> scales;

    Bone(const std::string& name, int id, const aiNodeAnim* channel);
};

glm::vec3 interpolatePosition(float animationTime, const Bone& bone);

glm::quat interpolateRotation(float animationTime, const Bone& bone);

glm::vec3 interpolateScale(float animationTime, const Bone& bone);

#endif 
