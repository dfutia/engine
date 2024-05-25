#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include "Graphics/bone.h"
#include "Graphics/boneinfo.h"

#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <vector>
#include <map>

struct Model;
struct aiAnimation;
struct aiNode;

struct AssimpNodeData {
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation {
public:
    float duration;
    int ticksPerSecond;
    AssimpNodeData rootNode;
    std::vector<Bone> bones;
    std::map<std::string, BoneInfo> boneInfoMap;

    Bone* findBone(const std::string& name);
    void readMissingBones(const aiAnimation* animation, Model& model);
    void readHierarchyData(AssimpNodeData& dest, const aiNode* src);
};

#endif 