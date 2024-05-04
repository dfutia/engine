#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb_image.h>
#include <spdlog/spdlog.h>

using TextureId = unsigned int;

TextureId loadTexture(const char* filePath);

#endif 