#include "sceneobject.h"

#include <glm/gtx/string_cast.hpp>
#include <spdlog/spdlog.h>

void printObject(SceneObject& object) {
	spdlog::info("Object Name: {}", object.name);
	spdlog::info("Object Position: {}", glm::to_string(object.position));
	spdlog::info("Object Rotation: {}", glm::to_string(object.rotation));
	spdlog::info("Object Scale: {}", glm::to_string(object.scale));
}