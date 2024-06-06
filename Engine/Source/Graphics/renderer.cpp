#include "renderer.h"

#include "Scene/scene.h"
#include "Asset/asset.h"
#include "Graphics/shader.h"
#include "animation.h"
#include "animator.h"

#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <spdlog/spdlog.h>

#include <unordered_set>

void renderScene(Scene& scene, float deltaTime) {
    glm::mat4 view = scene.camera->getViewMatrix();  // Get the dynamic view matrix from the camera
    glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)1280 / (float)720, 0.1f, 500.0f);  // Perspective projection matrix

    scene.program->use();
    scene.program->setUniform("projection", projection);
    scene.program->setUniform("view", view);

    // Render objects in the scene
    for (auto object : scene.objects) {
        static Animation danceAnimation("Assets/Animations/ReactionNoSkin.fbx", object->model.get());
        static Animator animator(&danceAnimation);

        animator.UpdateAnimation(deltaTime);

        // Local Space
        glm::vec3 position = object->position;
        glm::vec3 rotation = object->rotation;
        glm::vec3 scale = object->scale;

        // World Space
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position) *
            glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1)) *
            glm::scale(glm::mat4(1.0f), scale);

        scene.program->setUniform("model", model);

        // Bind Textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;

        for (unsigned int i = 0; i < object->model->textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number;
            std::string name = object->model->textures[i].type;

            if (name == "texture_diffuse") {
                number = std::to_string(diffuseNr++);
            }
            else if (name == "texture_specular") {
                number = std::to_string(specularNr++);
            }
            else if (name == "texture_normal") {
                number = std::to_string(normalNr++);
            }
            else if (name == "texture_height") {
                number = std::to_string(heightNr++);
            }

            scene.program->setUniformInt((name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, object->model->textures[i].id);
        }

        // Bind Mesh
        for (Mesh& mesh : object->model->meshes) {
            glBindVertexArray(mesh.vao);
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        auto transforms = animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i) {
            scene.program->setUniform("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }
    }
}