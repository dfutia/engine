#pragma once
#ifndef BONE_INFO_H
#define BONE_INFO_H

#include <glm/mat4x4.hpp>

// offset matrix = inverse bind pose matrix
// global inverse transformation is used to  undo default transformation of the root node. 
// The idea is to bring back model to origin with unit scale before animating. I haven't used it here because transformations of all mixamo models is identity

struct BoneInfo {
    int id;
    glm::mat4 offsetMatrix;

    BoneInfo() : id(-1), offsetMatrix(1.0f) {}
    BoneInfo(int id, const glm::mat4& offset) : id(id), offsetMatrix(offset) {}
};

#endif 