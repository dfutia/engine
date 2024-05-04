#pragma once
#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include "Graphics/model.h"

#include <glm/vec3.hpp>

#include <string>

struct SceneObject {
    std::string name;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    std::string model;
};

void printObject(SceneObject& object);

#endif 
