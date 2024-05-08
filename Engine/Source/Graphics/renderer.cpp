//#include "renderer.h"
//
//#include "Scene/scene.h"
//#include "Asset/asset.h"
//
//#include <glm/mat4x4.hpp>
//#include <glm/trigonometric.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#include <spdlog/spdlog.h>
//
//void renderScene(Scene& scene) {
//    // Render objects in the scene
//    for (SceneObject& object : scene.objects) {
//        // Local Space
//        glm::vec3 position = object.position;
//        glm::vec3 rotation = object.rotation;
//        glm::vec3 scale = object.scale;
//
//        // World Space
//        glm::mat4 model = glm::translate(glm::mat4(1.0f), position) *
//            glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0)) *
//            glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0)) *
//            glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1)) *
//            glm::scale(glm::mat4(1.0f), scale);
//        scene.sceneShader.setUniform("model", model);
//
//        // Bind Texture
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, loadTexture(gAssets, "Assets/Textures/container.jpg"));
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, loadTexture(gAssets, "Assets/Textures/awesomeface.png"));
//
//        // Bind Mesh
//        if (!object.model.empty()) {
//            Model model = loadModel(gAssets, object.model);
//            for (Mesh& mesh : model.meshes) {
//                glBindVertexArray(mesh.vao);
//                glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
//                glBindVertexArray(0);
//            }
//        }
//    }
//}