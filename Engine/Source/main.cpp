#include <spdlog/spdlog.h>

#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>

struct appInstance {
	SDL_Window* m_window = nullptr;
	SDL_GLContext m_glContext{};
} gApp;

void initWindow(appInstance app, const char* title, int width, int height, bool fullscreen) {
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

int main(int argc, char* argv[]) {
	initWindow(gApp, "Game", 1280, 720, true);


    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            }
        }
    }

    return 0;
}