#pragma once
#include "Include.h"

struct Transform {
	glm::vec3 position = { 0, 0, 0 };
	glm::vec4 rotation = { 0, 0, 0, 0 };
	glm::vec3 size = { 1, 1, 1 };
};