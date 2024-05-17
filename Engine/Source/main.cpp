#include "Asset/asset.h"
#include "Scene/scene.h"
#include "Graphics/renderer.h"

#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <spdlog/spdlog.h>
#include <glm/ext/matrix_clip_space.hpp>

struct App {
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_glContext{};
};

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
    App app;
    Scene scene;

	initWindow(app, "Game", 1280, 720, true);
    loadGameAssets();
    loadScene(scene);

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

        scene.camera->handleEvent(getFrameEvents(), deltaTime);

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glViewport(0, 0, 1280, 720);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderScene(scene, currentTime);

        lastTime = currentTime;
        getFrameEvents().clear();
        SDL_GL_SwapWindow(app.m_window);
    }

    shutdown(app);

    return 0;
}