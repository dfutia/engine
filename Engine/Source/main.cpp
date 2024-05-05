#include "Asset/asset.h"
#include "Scene/scene.h"
//#include "Graphics/renderer.h"
//#include "Graphics/texture.h"
//#include "Graphics/model.h"
#include "Graphics/shader.h"
#include "Graphics/camera.h"

#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <spdlog/spdlog.h>

struct App {
	SDL_Window* m_window = nullptr;
	SDL_GLContext m_glContext{};
} gApp;

void initWindow(App& app, const char* title, int width, int height, bool fullscreen) {
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

void shutdown(App& app) {
    SDL_GL_DeleteContext(app.m_glContext);
    SDL_DestroyWindow(app.m_window);
    SDL_Quit();
}

std::vector<SDL_Event>& getFrameEvents() {
    static std::vector<SDL_Event> frameEvents;
    return frameEvents;
}

int main(int argc, char* argv[]) {
	initWindow(gApp, "Game", 1280, 720, true);

    addTexture(gAssets, "Assets/Textures/container.jpg", loadTexture("Assets/Textures/container.jpg"));
    addTexture(gAssets, "Assets/Textures/awesomeface.png", loadTexture("Assets/Textures/awesomeface.png"));
    addModel(gAssets, "Assets/Meshes/uvcube.fbx", loadModel("Assets/Meshes/uvcube.fbx"));
    addModel(gAssets, "Assets/Meshes/suzanne.obj", loadModel("Assets/Meshes/suzanne.obj"));

    ShaderProgram solidColorShader = loadShaderProgram("Assets/Shaders/solidcolor.vert", "Assets/Shaders/solidcolor.frag");
    ShaderProgram textureShader = loadShaderProgram("Assets/Shaders/texture.vert", "Assets/Shaders/texture.frag");

    textureShader.use();
    textureShader.setUniformInt("texture1", 0);
    textureShader.setUniformInt("texture2", 1);

    Scene scene;
    Camera camera;

    SceneObject player;
    player.name = "Player";
    player.model = "Assets/Meshes/uvcube.fbx";
    player.position = glm::vec3(0.0f, 0.0f, 0.0f);
    player.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    player.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    scene.objects.push_back(player);

    scene.sceneShader = textureShader;

    Uint32 lastTime = SDL_GetTicks(), currentTime;

    bool running = true;
    while (running) {
        currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // Time in seconds

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            getFrameEvents().push_back(event);
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            }
        }

        camera.handleEvent(getFrameEvents(), deltaTime);

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glViewport(0, 0, 1280, 720);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getViewMatrix();  // Get the dynamic view matrix from the camera
        glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)1280 / (float)720, 0.1f, 500.0f);  // Perspective projection matrix

        textureShader.use();
        textureShader.setUniform("projection", projection);
        textureShader.setUniform("view", view);

        

        lastTime = currentTime;
        getFrameEvents().clear();
        SDL_GL_SwapWindow(gApp.m_window);
    }

    shutdown(gApp);

    return 0;
}