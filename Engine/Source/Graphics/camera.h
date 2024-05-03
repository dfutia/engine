#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <SDL.h>

#include <vector>

class Camera {
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);
	~Camera() = default;

	void handleEvent(const std::vector<SDL_Event>& events, float deltaTime);

	glm::mat4 getViewMatrix();
private:
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_worldUp;

	float m_yaw;
	float m_pitch;

	void handleMouseMovement(float xoffset, float yoffset);
	void updateCameraVectors();
};

#endif 
