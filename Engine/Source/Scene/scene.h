#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "Scene/sceneobject.h"
#include "Graphics/shader.h"
#include "Asset/asset.h"

#include <vector>

struct Scene {
	std::vector<SceneObject> objects;
	ShaderProgram sceneShader = loadShader(gAssets, "Assets/Shaders/texture.vert", "Assets/Shaders/texture.frag");
};

#endif 
