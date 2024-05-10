#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Scene;
struct ShaderProgram;

void renderScene(Scene& scene);

#endif 