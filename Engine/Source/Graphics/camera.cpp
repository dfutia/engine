#include "camera.h"

#include <spdlog/spdlog.h>

void Camera::handleEvent(const std::vector<SDL_Event>& events) {
    for (SDL_Event event : events) {
        switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_w:  
                spdlog::info("Forward");
                break;
            case SDLK_s:
                spdlog::info("Backward");
                break;
            case SDLK_a: 
                spdlog::info("Left");
                break;
            case SDLK_d:
                spdlog::info("Right");
                break;
            }
            break;

        case SDL_MOUSEWHEEL:
            if (event.wheel.y > 0)
                spdlog::info("Zoom in");
            else if (event.wheel.y < 0) 
                spdlog::info("Zoom out");
            break;
        }
}