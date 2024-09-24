#pragma once

struct Transform
{
	glm::vec3 position{ 0.f };
	glm::mat4 proj{ 1.f };
	glm::mat4 view{ 1.f };
	glm::mat4 model{ 1.f };
};