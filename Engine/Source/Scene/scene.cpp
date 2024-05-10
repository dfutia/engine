#include "scene.h"

void addObjectToScene(Scene& scene, std::shared_ptr<SceneObject> object) {
    scene.objects.push_back(object);
}

void loadScene(Scene& scene) {
    scene.camera = std::make_shared<Camera>();
    scene.program = loadShader(gAssets, "Assets/Shaders/texture.vert", "Assets/Shaders/texture.frag");

    auto player = std::make_shared<SceneObject>();
    player->name = "Player";
    player->model = loadModel(gAssets, "Assets/Meshes/Maria/Maria J J Ong.dae");
    player->position = glm::vec3(0.0f, 0.0f, 0.0f);
    player->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    player->scale = glm::vec3(0.1f, 0.1f, 0.1f);

    addObjectToScene(scene, player);
}
