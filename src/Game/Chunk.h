#pragma once

#include <glm/glm.hpp>
#include <FastNoise/FastNoiseLite.h>
#include <array>
#include <vector>
#include <thread>
#include <mutex>

#include "../OpenGL/Shader.h"
#include "../OpenGL/Buffer.h"
#include "../OpenGL/Texture.h"
#include "ObjectComponents.h"
#include "FrustumCulling.h"

// Global
constexpr uint8_t CHUNK_SIZE = 8;
constexpr uint8_t CHUNK_HEIGHT = 255;

constexpr uint32_t POSITION_X_MASK = 0x1F;
constexpr uint32_t POSITION_Z_MASK = 0x1F;
constexpr uint32_t POSITION_Y_MASK = 0xFF;
constexpr uint32_t COLOR_INDEX_MASK = 0x7;
constexpr uint32_t NORMAL_INDEX_MASK = 0x7;
constexpr uint32_t AMBIENT_LEVEL_MASK = 0x3;

struct Vertex
{
	uint32_t packedData;
};

class Chunk
{
public:
	Chunk(glm::ivec2 position = glm::ivec3(0.f), FastNoiseLite noise=NULL, bool shouldBuild = false);
	~Chunk();

	const uint32_t PackVertexData(int8_t x, int8_t y, int8_t z, uint8_t colorIndex, uint8_t normalIndex, uint8_t ambientLevel) const;
	void UnpackVertexData(uint32_t packedData, int8_t& x, int8_t& y, int8_t& z, uint8_t& colorIndex, uint8_t& normalIndex, uint8_t& ambientLevel);
	const std::array<uint8_t, 4> AddAO(uint8_t normalIndex, glm::ivec3 facePos) const;

	void CleanChunk();
	void GenerateChunkT(glm::ivec2 position, FastNoiseLite& noise);	// T -> Thread
	void BuildChunk();

	int GetBlockIndex(const int32_t x, const int y, const int32_t z) const;
	bool GetBlock(const int x, const int y, const int z) const;
	bool OutOfBounds(const int x, const int y, const int z);
	bool OutOfBounds(const int value, const int max = CHUNK_SIZE);

	void AddBlock(const int32_t x, const int y, const int32_t z);
	void TryAddFace(const std::array<Vertex, 6> &blockFace, const glm::ivec3 &facePos, const glm::ivec3 &adjacentBlock);
	void AddFace(const std::array<Vertex, 6> &blockFace, const glm::ivec3 &facePos);
	
	void Draw();

	Transform transform{};
	AABB aabb;

	std::atomic<bool> isCleaned{ false };
	std::atomic<bool> isChunkLoaded{ false };

private:
	std::array<bool, CHUNK_SIZE*CHUNK_HEIGHT*CHUNK_SIZE> m_Blocks;
	std::vector<Vertex> m_Vertices;

	FastNoiseLite m_Noise;
	Shader m_Shader;
	Buffer m_VBO{ {GL_ARRAY_BUFFER, NULL, CHUNK_SIZE * CHUNK_SIZE * sizeof(Vertex), GL_DYNAMIC_DRAW}};
	Texture m_GrassTex{"res/textures/grass.jpg"};
	unsigned int m_VAO;

	bool generateGLData = true;
	std::thread m_ChunkThread;
	std::mutex mtx;
};