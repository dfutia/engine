#include "Scene/scene.h"
#include "Graphics/renderer.h"
#include "Graphics/model.h"
#include "Graphics/shader.h"

#include <spdlog/spdlog.h>

#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>

struct appInstance {
	SDL_Window* m_window = nullptr;
	SDL_GLContext m_glContext{};
} gApp;

void initWindow(appInstance& app, const char* title, int width, int height, bool fullscreen) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        spdlog::error("SDL Init {}", SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    const Uint32 windowFlags = (SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_RESIZABLE : 0));
    app.m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
    if (!app.m_window) {
        spdlog::error("SDL Create Window {}", SDL_GetError());
    }

    app.m_glContext = SDL_GL_CreateContext(app.m_window);
    if (!app.m_glContext) {
        spdlog::error(" SDL GL Context {}", SDL_GetError());
        SDL_DestroyWindow(app.m_window);
        SDL_Quit();
    }

    if (!gladLoadGL()) {
        spdlog::error("GLAD Init {}", SDL_GetError());
        SDL_GL_DeleteContext(app.m_glContext);
        SDL_DestroyWindow(app.m_window);
        SDL_Quit();
    }
}

std::vector<SDL_Event>& getFrameEvents() {
    static std::vector<SDL_Event> frameEvents;
    return frameEvents;
}

int main(int argc, char* argv[]) {
	initWindow(gApp, "Game", 1280, 720, true);

    Scene scene;

    Model model = loadModel("Assets/Meshes/cube.obj");
    ShaderProgram shader = loadShaderProgram("Assets/Shaders/basic.vert", "Assets/Shaders/basic.frag");

    SceneObject player;
    player.name = "Player";
    player.model = model;
    player.position = glm::vec3(0.0f, 0.0f, 0.0f);
    player.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    player.scale = glm::vec3(1.0f, 1.0f, 1.0f);

    scene.objects.push_back(player);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            default:
                getFrameEvents().push_back(event);
                break;
            }
        }

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glViewport(0, 0, 1280, 720);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(70.0f), (float)1280 / (float)720, 0.1f, 500.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

        shader.use();
        shader.setUniform("projection", projection);
        shader.setUniform("view", view);

        // Render objects in the scene
        for (SceneObject& object : scene.objects) {
            // Local Space
            glm::vec3 position = object.position;
            glm::vec3 rotation = object.rotation;
            glm::vec3 scale = object.scale;

            // World Space
            glm::mat4 model = glm::translate(glm::mat4(1.0f), position) *
                glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0)) *
                glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0)) *
                glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1)) *
                glm::scale(glm::mat4(1.0f), scale);
            shader.setUniform("model", model);

            for (Mesh& mesh : object.model.meshes) {
                glBindVertexArray(mesh.vao);
                glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }

        SDL_GL_SwapWindow(gApp.m_window);
        getFrameEvents().clear();
    }

    return 0;
}