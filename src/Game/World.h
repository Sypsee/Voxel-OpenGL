#pragma once

#include "Chunk.h"
#include "../Core/Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include <FastNoise/FastNoise.h>

#include <unordered_map>

class World
{
public:
	World();

	bool GetBlock(const int x, const int y, const int z);

	void Draw(const Camera &cam);

private:
	std::unordered_map<glm::ivec2, Chunk> chunks;
	
	std::array<float, CHUNK_SIZE*CHUNK_SIZE> noiseOutput;
	int seed = rand();

	const int16_t chunkDiameter = 50;
	const int8_t m_RenderDistance = 4;
};