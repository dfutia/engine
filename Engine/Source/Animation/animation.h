#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include "Animation/boneinfo.h"
#include "Animation/bone.h"

#include <assimp/scene.h>

#include <string>
#include <vector>
#include <map>

struct Animation {
    double duration;
    double ticksPerSecond;
    std::vector<Bone> bones;
    std::map<int, std::vector<int>> boneHierarchy; // Map from parent bone ID to children bone IDs

    Animation(const aiScene* scene, const std::map<std::string, BoneInfo>& boneInfoMap);
};



#endif 