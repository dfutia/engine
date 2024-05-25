#pragma once
#ifndef BONE_INFO_H
#define BONE_INFO_H

#include <glm/mat4x4.hpp>

struct BoneInfo {
    int id;
    glm::mat4 offsetMatrix;
};

#endif 
