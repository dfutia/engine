#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <SDL.h>

#include <vector>

class Camera {
public:
	Camera() = default;
	~Camera() = default;

	void handleEvent(const std::vector<SDL_Event>& events);
private:
};

#endif 
