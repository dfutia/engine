#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "Asset/asset.h"
#include "Scene/sceneobject.h"
#include "Graphics/shader.h"
#include "Graphics/camera.h"

#include <vector>
#include <functional>

struct Scene {
	std::vector<std::shared_ptr<SceneObject>> objects;
	std::shared_ptr<Camera> camera;
    std::shared_ptr<ShaderProgram> program;
};

void addObjectToScene(Scene& scene, std::shared_ptr<SceneObject> object);
void loadScene(Scene& scene);

#endif 
