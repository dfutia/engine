#include "Core/time.h"
#include "Asset/asset.h"
#include "Scene/scene.h"
#include "Graphics/renderer.h"

#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <spdlog/spdlog.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <stb_image.h>

struct App {
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_glContext{};
};

void initWindow(App& app, const char* title, int width, int height, bool fullscreen) {
    const Uint32 sdlFlags = (SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
    if (SDL_Init(sdlFlags) != 0) {
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

float getDeltaTime() {
    static Uint32 lastTime = 0;
    Uint32 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;
    return deltaTime;
}

int main(int argc, char* argv[]) {
    App app;
    Scene scene;

	initWindow(app, "Game", 1280, 720, true);

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    loadGameAssets();
    loadScene(scene);

    Time time;
    time.initialize();

    Timer timer;

    uint32_t lastTick = 0, currentTick;
    auto lastLogTime = std::chrono::high_resolution_clock::now();

    bool running = true;

    while (running) {
        currentTick = time.tick();
        if (currentTick != lastTick) {
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = now - lastLogTime;
            spdlog::info("Tick. Time since last tick: {:.6f} seconds", elapsed.count());
            lastLogTime = now;
            lastTick = currentTick;
        }

        float delta = timer.getDelta();
        timer.reset();
        spdlog::info("Delta: {:.6f}", delta);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            getFrameEvents().push_back(event);
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            }
        }

        scene.camera->handleEvent(getFrameEvents(), delta);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glViewport(0, 0, 1280, 720);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderScene(scene, delta);

        getFrameEvents().clear();
        SDL_GL_SwapWindow(app.m_window);

        time.wait(1); // Reduce CPU usage
    }

    shutdown(app);

    return 0;
}

