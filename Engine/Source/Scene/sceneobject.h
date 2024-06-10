#pragma once
#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <glm/vec3.hpp>

#include <memory>
#include <functional>
#include <string>

struct Model;
struct Animator;

struct SceneObject {
    std::string name;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    Model* model;
};

void printObject(SceneObject& object);

#endif 