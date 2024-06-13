#include "scene.h"
#include "Graphics/animator.h"

#include <spdlog/spdlog.h>

void addObjectToScene(Scene& scene, std::shared_ptr<SceneObject> object) {
    scene.objects.push_back(object);
}

void loadScene(Scene& scene) {
    scene.camera = std::make_shared<Camera>();
    //scene.program = loadShader(gAssets, "Assets/Shaders/texture.vert", "Assets/Shaders/texture.frag");
    scene.program = loadShader(gAssets, "Assets/Shaders/skinned.vert", "Assets/Shaders/texture.frag");

    auto player = std::make_shared<SceneObject>();
    player->name = "Player";
    player->position = glm::vec3(0.0f, 0.0f, 0.0f);
    player->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    player->scale = glm::vec3(0.1f);
    player->model = loadModel(gAssets, "Assets/Meshes/Maria J J Ong.fbx");
    player->animator = new Animator(loadAnimation(gAssets, "Assets/Animations/Twist Dance.fbx"), player->model);

    player->animator->PlayAnimation(loadAnimation(gAssets, "Assets/Animations/Dying (1).fbx"), player->model);

    addObjectToScene(scene, player);
}
