#pragma once

#include <glm/glm.hpp>
#include <array>
#include <vector>

#include "../OpenGL/Shader.h"
#include "../OpenGL/Buffer.h"
#include "../OpenGL/Texture.h"
#include "ObjectComponents.h"

#define CHUNK_SIZE 16

class Chunk
{
public:
	Chunk(glm::ivec2 position = glm::ivec3(0.f), const std::array<float, CHUNK_SIZE*CHUNK_SIZE>& noiseOutput={});
	~Chunk();

	void CleanChunk();
	void BuildChunk(glm::ivec2 position, const std::array<float, CHUNK_SIZE* CHUNK_SIZE>& noiseOutput);

	int GetBlockIndex(const int8_t x, const int y, const int8_t z) const;
	bool GetBlock(const int x, const int y, const int z) const;
	bool OutOfBounds(const int x, const int y, const int z);
	bool OutOfBounds(const int value);

	void AddBlock(const int8_t x, const int y, const int8_t z);
	void TryAddFace(const std::array<float, 20> &blockFace, const glm::ivec3 &facePos, const glm::ivec3 &adjacentBlock);
	void AddFace(const std::array<float, 20> &blockFace, const glm::ivec3 &facePos);
	
	void Draw();

	Transform transform{};
	bool isCleaned = false;
	bool isChunkLoaded = false;

private:
	bool m_Blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;

	Shader m_Shader;
	Buffer m_VBO{ {GL_ARRAY_BUFFER, NULL, m_Vertices.size() * sizeof(float), GL_DYNAMIC_DRAW} };
	Buffer m_IBO{ {GL_ELEMENT_ARRAY_BUFFER, NULL, m_Indices.size() * sizeof(unsigned int), GL_DYNAMIC_DRAW} };
	Texture grassTex{"res/textures/grass.jpg"};
	unsigned int m_VAO;
};