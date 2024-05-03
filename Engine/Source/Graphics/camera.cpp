#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : m_front(glm::vec3(0.0f, 0.0f, -1.0f)) {
    m_position = position;
    m_worldUp = up;
    m_yaw = yaw;
    m_pitch = pitch;

    updateCameraVectors();
}

void Camera::handleEvent(const std::vector<SDL_Event>& events, float deltaTime) {
    static float moveSpeed = 10.0f;
    static glm::vec3 moveDirection(0.0f);
    static bool keysPressed[4] = { false, false, false, false }; // Track key states (W, S, A, D)
    static bool rightMouseButtonPressed = false; // Track the state of the right mouse button

    // Reset the movement direction at the beginning of each frame
    moveDirection = glm::vec3(0.0f);

    for (const SDL_Event& event : events) {
        switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_w:
                keysPressed[0] = true;
                break;
            case SDLK_s:
                keysPressed[1] = true;
                break;
            case SDLK_a:
                keysPressed[2] = true;
                break;
            case SDLK_d:
                keysPressed[3] = true;
                break;
            }
            break;

        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            case SDLK_w:
                keysPressed[0] = false;
                break;
            case SDLK_s:
                keysPressed[1] = false;
                break;
            case SDLK_a:
                keysPressed[2] = false;
                break;
            case SDLK_d:
                keysPressed[3] = false;
                break;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_RIGHT) {
                rightMouseButtonPressed = true;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_RIGHT) {
                rightMouseButtonPressed = false;
            }
            break;

        case SDL_MOUSEMOTION:
            if (rightMouseButtonPressed) {
                float xoffset = static_cast<float>(event.motion.xrel);
                float yoffset = static_cast<float>(event.motion.yrel);
                handleMouseMovement(xoffset, yoffset);
            }
            break;
        }
    }

    // Update the move direction based on the current key states
    if (keysPressed[0]) moveDirection += m_front;
    if (keysPressed[1]) moveDirection -= m_front;
    if (keysPressed[2]) moveDirection -= m_right;
    if (keysPressed[3]) moveDirection += m_right;

    // Update the camera position based on the move direction and elapsed time
    m_position += moveDirection * moveSpeed * deltaTime;
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::handleMouseMovement(float xoffset, float yoffset) {
    static const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= -sensitivity; // Negate yoffset to invert the vertical movement

    m_yaw += xoffset;
    m_pitch += yoffset;

    // Make sure the pitch is within the range [-89.0f, 89.0f]
    m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}