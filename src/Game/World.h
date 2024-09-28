#pragma once

#include "Chunk.h"
#include "../Core/Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include <FastNoise/FastNoiseLite.h>

#include <unordered_map>
#include <chrono>

class World
{
public:
	World();

	bool GetBlock(const int x, const int y, const int z);

	void Draw(const Camera &cam);

private:
	std::unordered_map<glm::ivec2, Chunk> chunks;

	FastNoiseLite noise;
	int seed = rand();

	const int16_t chunkDiameter = 1;
	const int8_t m_RenderDistance = 12;
	int8_t m_LoadDistance = 2;

	FrustumCulling fc;

	std::chrono::time_point<std::chrono::high_resolution_clock> previousTime;
};