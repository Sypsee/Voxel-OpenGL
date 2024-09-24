#pragma once

#include <glm/glm.hpp>
#include <FastNoise/FastNoiseLite.h>
#include <array>
#include <vector>
#include <thread>
#include <algorithm>
#include <execution>

#include "../OpenGL/Shader.h"
#include "../OpenGL/Buffer.h"
#include "../OpenGL/Texture.h"
#include "ObjectComponents.h"

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 256

class Chunk
{
public:
	Chunk(glm::ivec2 position = glm::ivec3(0.f), FastNoiseLite noise=NULL, bool shouldBuild = false);
	~Chunk();

	void CleanChunk();
	void GenerateChunkT(glm::ivec2 position, FastNoiseLite& noise);	// T -> Thread
	void BuildChunk();

	int GetBlockIndex(const int32_t x, const int y, const int32_t z) const;
	bool GetBlock(const int x, const int y, const int z) const;
	bool OutOfBounds(const int x, const int y, const int z);
	bool OutOfBounds(const int value, const int max = CHUNK_SIZE);

	void AddBlock(const int32_t x, const int y, const int32_t z);
	void TryAddFace(const std::array<float, 20> &blockFace, const glm::ivec3 &facePos, const glm::ivec3 &adjacentBlock);
	void AddFace(const std::array<float, 20> &blockFace, const glm::ivec3 &facePos);
	
	void Draw();

	Transform transform{};
	bool isCleaned = false;
	bool isChunkLoaded = false;

private:
	std::array<bool, CHUNK_SIZE*CHUNK_HEIGHT*CHUNK_SIZE> m_Blocks;
	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::vector<int32_t> xCoord;	// For thread

	FastNoiseLite m_Noise;
	Shader m_Shader;
	Buffer m_VBO{ {GL_ARRAY_BUFFER, NULL, m_Vertices.size() * sizeof(float), GL_DYNAMIC_DRAW} };
	Buffer m_IBO{ {GL_ELEMENT_ARRAY_BUFFER, NULL, m_Indices.size() * sizeof(unsigned int), GL_DYNAMIC_DRAW} };
	Texture m_GrassTex{"res/textures/grass.jpg"};
	unsigned int m_VAO;

	std::thread m_ChunkThread;
};