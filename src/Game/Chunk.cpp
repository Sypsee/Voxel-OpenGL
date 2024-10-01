#include "Chunk.h"

#include <cmath>

int32_t FloatToIntNoise(float val)
{
	return static_cast<int32_t>(((val + 1) / 2) * 55);
}

Chunk::Chunk(glm::ivec2 position, FastNoiseLite noise, bool shouldBuild)
	:m_Blocks{0}, aabb{glm::vec3(position.x / CHUNK_SIZE - 10, 0, position.y / CHUNK_SIZE - 10), glm::vec3(position.x + 10, 0, position.y + 10) + glm::vec3(CHUNK_SIZE, CHUNK_HEIGHT, CHUNK_SIZE)}
{
	this->m_Noise = noise;
	transform.position = { position.x, 0, position.y };
	transform.model = glm::translate(transform.model, glm::vec3(position.x, 0, position.y));

	m_Shader.AttachShader({ "res/shaders/cube.frag", GL_FRAGMENT_SHADER });
	m_Shader.AttachShader({ "res/shaders/cube.vert", GL_VERTEX_SHADER });

	m_GrassTex.Bind();
	m_Shader.setI("tex", 0);

	glGenVertexArrays(1, &m_VAO);

	if (!shouldBuild) return;

	m_ChunkThread = std::thread(&Chunk::BuildChunk, this);
}

Chunk::~Chunk()
{
	CleanChunk();
}

const uint32_t Chunk::PackVertexData(int8_t x, int8_t y, int8_t z, uint8_t colorIndex, uint8_t normalIndex, uint8_t ambientLevel) const {
	uint32_t packedData = 0;
	packedData |= (static_cast<uint32_t>(x) & POSITION_X_MASK);
	packedData |= ((static_cast<uint32_t>(y) & POSITION_Y_MASK) << 5);
	packedData |= ((static_cast<uint32_t>(z) & POSITION_Z_MASK) << 13);
	packedData |= ((static_cast<uint32_t>(colorIndex) & COLOR_INDEX_MASK) << 18);
	packedData |= ((static_cast<uint32_t>(normalIndex) & NORMAL_INDEX_MASK) << 21);
	packedData |= ((static_cast<uint32_t>(ambientLevel) & AMBIENT_LEVEL_MASK) << 24);
	return packedData;
}

void Chunk::UnpackVertexData(uint32_t packedData, int8_t& x, int8_t& y, int8_t& z, uint8_t& colorIndex, uint8_t& normalIndex, uint8_t& ambientLevel) {
	x = packedData & POSITION_X_MASK;
	y = (packedData >> 5) & POSITION_Y_MASK;
	z = (packedData >> 13) & POSITION_Z_MASK;
	colorIndex = (packedData >> 18) & COLOR_INDEX_MASK;
	normalIndex = (packedData >> 21) & NORMAL_INDEX_MASK;
	ambientLevel = (packedData >> 24) & AMBIENT_LEVEL_MASK;
}

const std::array<uint8_t, 4> Chunk::AddAO(uint8_t normalIndex, glm::ivec3 facePos) const
{
	std::array<uint8_t, 4> ao = { 0,0,0 };

	if (normalIndex == 4 || normalIndex == 5)	// Top & Bottom Face
	{
		bool a = GetBlock(facePos.x, facePos.y, facePos.z - 1);
		bool b = GetBlock(facePos.x - 1, facePos.y, facePos.z - 1);
		bool c = GetBlock(facePos.x - 1, facePos.y, facePos.z);
		bool d = GetBlock(facePos.x - 1, facePos.y, facePos.z + 1);
		bool e = GetBlock(facePos.x, facePos.y, facePos.z + 1);
		bool f = GetBlock(facePos.x + 1, facePos.y, facePos.z + 1);
		bool g = GetBlock(facePos.x + 1, facePos.y, facePos.z);
		bool h = GetBlock(facePos.x + 1, facePos.y, facePos.z - 1);

		ao[0] = a + b + c;
		ao[1] = g + h + a;
		ao[2] = e + f + g;
		ao[3] = c + d + e;
	}
	else if (normalIndex == 2 || normalIndex == 3)	// Right & Left Face
	{
		bool a = GetBlock(facePos.x, facePos.y, facePos.z - 1);
		bool b = GetBlock(facePos.x, facePos.y - 1, facePos.z - 1);
		bool c = GetBlock(facePos.x, facePos.y - 1, facePos.z);
		bool d = GetBlock(facePos.x, facePos.y - 1, facePos.z + 1);
		bool e = GetBlock(facePos.x, facePos.y, facePos.z + 1);
		bool f = GetBlock(facePos.x, facePos.y + 1, facePos.z + 1);
		bool g = GetBlock(facePos.x, facePos.y + 1, facePos.z);
		bool h = GetBlock(facePos.x, facePos.y + 1, facePos.z - 1);

		ao[0] = a + b + c;
		ao[1] = g + h + a;
		ao[2] = e + f + g;
		ao[3] = c + d + e;
	}
	else // Front & Back Face
	{
		bool a = GetBlock(facePos.x - 1, facePos.y, facePos.z);
		bool b = GetBlock(facePos.x - 1, facePos.y - 1, facePos.z);
		bool c = GetBlock(facePos.x, facePos.y - 1, facePos.z);
		bool d = GetBlock(facePos.x + 1, facePos.y - 1, facePos.z);
		bool e = GetBlock(facePos.x + 1, facePos.y, facePos.z);
		bool f = GetBlock(facePos.x + 1, facePos.y + 1, facePos.z);
		bool g = GetBlock(facePos.x - 1, facePos.y + 1, facePos.z);
		bool h = GetBlock(facePos.x - 1, facePos.y + 1, facePos.z);

		ao[0] = a + b + c;
		ao[1] = g + h + a;
		ao[2] = e + f + g;
		ao[3] = c + d + e;
	}

	ao[0] = std::min(ao[0], (uint8_t)3);
	ao[1] = std::min(ao[1], (uint8_t)3);
	ao[2] = std::min(ao[2], (uint8_t)3);
	ao[3] = std::min(ao[3], (uint8_t)3);

	return ao;
}

void Chunk::CleanChunk()
{
	if (m_ChunkThread.joinable())
		m_ChunkThread.join();

	m_Blocks.fill(0);
	m_Vertices.clear();
	m_Noise = NULL;

	glDeleteVertexArrays(1, &m_VAO);
	m_Shader.DestroyShader();
	m_VBO.DestroyBuffer();

	isCleaned = true;
	isChunkLoaded = false;
}

void Chunk::GenerateChunkT(glm::ivec2 position, FastNoiseLite& noise)
{
	this->m_Noise = noise;
	transform.model = glm::translate(transform.model, glm::vec3(position.x, 0, position.y));
	transform.position = { position.x, 0, position.y };

	if (m_ChunkThread.joinable()) {
		m_ChunkThread.join();
	}

	//m_ChunkThread = std::thread(&Chunk::BuildChunk, this);
	BuildChunk();
}

void Chunk::BuildChunk()
{
	for (int32_t x = 0; x < CHUNK_SIZE; x++)
	{
		float noiseX = (transform.position.x + static_cast<float>(x));
		for (int32_t z = 0; z < CHUNK_SIZE; z++)
		{
			float noiseY = (transform.position.z + static_cast<float>(z));

			for (int32_t y = 0; y < CHUNK_HEIGHT; y++)
			{
				float noiseValue = m_Noise.GetNoise(noiseX, (float)y, noiseY);
				int32_t height = FloatToIntNoise(noiseValue);
				if (height < y) continue;

				if (OutOfBounds(x, y, z)) break;
				m_Blocks[GetBlockIndex(x, y, z)] = true;
			}
		}
	}

	for (int32_t x = 0; x < CHUNK_SIZE; x++)
	{
		float noiseX = (transform.position.x + static_cast<float>(x));
		for (int32_t z = 0; z < CHUNK_SIZE; z++)
		{
			float noiseY = (transform.position.z + static_cast<float>(z));

			for (int32_t y = 0; y < CHUNK_HEIGHT; y++)
			{
				float noiseValue = m_Noise.GetNoise(noiseX, (float)y, noiseY);
				int32_t height = FloatToIntNoise(noiseValue);
				if (height < y) continue;

				if (OutOfBounds(x, y, z)) break;
				AddBlock(x, y, z);
			}
		}
	}

	isChunkLoaded = true;
	isCleaned = false;
}

int Chunk::GetBlockIndex(const int32_t x, const int y, const int32_t z) const
{
	return z * CHUNK_SIZE * CHUNK_HEIGHT + y * CHUNK_SIZE + x;
}

bool Chunk::GetBlock(const int x, const int y, const int z) const
{
	if (x < 0 || x >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT) return false;
	return m_Blocks[GetBlockIndex(x, y, z)];
}

bool Chunk::OutOfBounds(const int x, const int y, const int z)
{
	return OutOfBounds(x) || OutOfBounds(y, CHUNK_HEIGHT) || OutOfBounds(z);
}

bool Chunk::OutOfBounds(const int value, const int max)
{
	return value < 0 || value >= max;
}

void Chunk::AddBlock(const int32_t x, const int y, const int32_t z)
{
	srand(GetBlockIndex(x, y, z));
	uint8_t randomColorIndex = rand() % 5;

	const std::array<Vertex, 6> frontFace = {
		Vertex{ PackVertexData(0, 0, 0, randomColorIndex, 0, 0) },
		Vertex{ PackVertexData(1, 0, 0, randomColorIndex, 0, 0) },
		Vertex{ PackVertexData(1, 1, 0, randomColorIndex, 0, 0) },
		Vertex{ PackVertexData(1, 1, 0, randomColorIndex, 0, 0) },
		Vertex{ PackVertexData(0, 1, 0, randomColorIndex, 0, 0) },
		Vertex{ PackVertexData(0, 0, 0, randomColorIndex, 0, 0) }
	};

	const std::array<Vertex, 6> backFace = {
		Vertex{ PackVertexData(0, 0, 1, randomColorIndex, 1, 0) },
		Vertex{ PackVertexData(1, 0, 1, randomColorIndex, 1, 0) },
		Vertex{ PackVertexData(1, 1, 1, randomColorIndex, 1, 0) },
		Vertex{ PackVertexData(1, 1, 1, randomColorIndex, 1, 0) },
		Vertex{ PackVertexData(0, 1, 1, randomColorIndex, 1, 0) },
		Vertex{ PackVertexData(0, 0, 1, randomColorIndex, 1, 0) }
	};

	const std::array<Vertex, 6> leftFace = {
		Vertex{ PackVertexData(0, 1, 1, randomColorIndex, 2, 0) },
		Vertex{ PackVertexData(0, 1, 0, randomColorIndex, 2, 0) },
		Vertex{ PackVertexData(0, 0, 0, randomColorIndex, 2, 0) },
		Vertex{ PackVertexData(0, 0, 0, randomColorIndex, 2, 0) },
		Vertex{ PackVertexData(0, 0, 1, randomColorIndex, 2, 0) },
		Vertex{ PackVertexData(0, 1, 1, randomColorIndex, 2, 0) }
	};

	const std::array<Vertex, 6> rightFace = {
		Vertex{ PackVertexData(1, 1, 1, randomColorIndex, 3, 0) },
		Vertex{ PackVertexData(1, 1, 0, randomColorIndex, 3, 0) },
		Vertex{ PackVertexData(1, 0, 0, randomColorIndex, 3, 0) },
		Vertex{ PackVertexData(1, 0, 0, randomColorIndex, 3, 0) },
		Vertex{ PackVertexData(1, 0, 1, randomColorIndex, 3, 0) },
		Vertex{ PackVertexData(1, 1, 1, randomColorIndex, 3, 0) }
	};

	const std::array<Vertex, 6> bottomFace = {
		Vertex{ PackVertexData(0, 0, 0, randomColorIndex, 4, 0) },
		Vertex{ PackVertexData(1, 0, 0, randomColorIndex, 4, 0) },
		Vertex{ PackVertexData(1, 0, 1, randomColorIndex, 4, 0) },
		Vertex{ PackVertexData(1, 0, 1, randomColorIndex, 4, 0) },
		Vertex{ PackVertexData(0, 0, 1, randomColorIndex, 4, 0) },
		Vertex{ PackVertexData(0, 0, 0, randomColorIndex, 4, 0) }
	};

	const std::array<Vertex, 6> topFace = {
		Vertex{ PackVertexData(0, 1, 0, randomColorIndex, 5, 0) },
		Vertex{ PackVertexData(1, 1, 0, randomColorIndex, 5, 0) },
		Vertex{ PackVertexData(1, 1, 1, randomColorIndex, 5, 0) },
		Vertex{ PackVertexData(1, 1, 1, randomColorIndex, 5, 0) },
		Vertex{ PackVertexData(0, 1, 1, randomColorIndex, 5, 0) },
		Vertex{ PackVertexData(0, 1, 0, randomColorIndex, 5, 0) }
	};


	glm::ivec3 facePos = { x,y,z };
	TryAddFace(frontFace, facePos, { x, y, z - 1 });
	TryAddFace(backFace, facePos, { x, y, z + 1 });
	TryAddFace(leftFace, facePos, { x - 1, y, z });
	TryAddFace(rightFace, facePos, { x + 1, y, z });
	if (y != 0 || OutOfBounds(x, y - 1, z))
		TryAddFace(bottomFace, facePos, { x, y - 1, z });
	TryAddFace(topFace, facePos, { x, y + 1, z});
}

void Chunk::TryAddFace(const std::array<Vertex, 6>& blockFace, const glm::ivec3& facePos, const glm::ivec3& adjacentBlock)
{
	if (OutOfBounds(adjacentBlock.x, adjacentBlock.y, adjacentBlock.z) ||
		!m_Blocks[GetBlockIndex(adjacentBlock.x, adjacentBlock.y, adjacentBlock.z)])
	{
		AddFace(blockFace, facePos);
	}
}

void Chunk::AddFace(const std::array<Vertex, 6> &blockFace, const glm::ivec3 &facePos)
{
	try {
		std::lock_guard<std::mutex> lock(mtx);

		// Repack vertex data and add ambient + convert [local pos -> world pos]
		for (int i=0; i<blockFace.size(); i++)
		{
			const Vertex vert = blockFace[i];

			int8_t x = 0;
			int8_t y = 0;
			int8_t z = 0;
			uint8_t colorIndex = 0;
			uint8_t normalIndex = 0;
			uint8_t ambientLevel = 0;

			UnpackVertexData(vert.packedData, x, y, z, colorIndex, normalIndex, ambientLevel);

			std::array<uint8_t, 4> ao = AddAO(normalIndex, facePos);

			if (i == 0 || i == 5)
			{
				ambientLevel = ao[0];
			}
			else if (i == 1)
			{
				ambientLevel = ao[1];
			}
			else if (i == 2 || i == 3)
			{
				ambientLevel = ao[2];
			}
			else if (i == 4)
			{
				ambientLevel = ao[3];
			}

			m_Vertices.emplace_back(
				Vertex{
					PackVertexData(static_cast<int8_t>(facePos.x) + x,
						static_cast<int8_t>(facePos.y) + y,
						static_cast<int8_t>(facePos.z) + z,
						colorIndex,
						normalIndex,
						ambientLevel
					)
				}
			);
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in AddFace: " << e.what() << std::endl;
	}
}

void Chunk::Draw()
{
	if (!isChunkLoaded) return;

	if (generateGLData)
	{
		glBindVertexArray(m_VAO);

		m_VBO.UploadData(&m_Vertices[0], m_Vertices.size() * sizeof(Vertex));

		glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::packedData));
		glEnableVertexAttribArray(0);

		generateGLData = false;
	}

	m_Shader.Bind();
	m_Shader.setMat4("model", transform.model);
	m_Shader.setMat4("view", transform.view);
	m_Shader.setMat4("proj", transform.proj);
	m_Shader.setVec3("viewPos", transform.camPos);

	float normalizedTime = static_cast<float>(fmod(glfwGetTime(), 10)) / 10;
	m_Shader.setF("time", normalizedTime/50);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, m_Vertices.size());
	glBindVertexArray(0);

	m_Shader.UnBind();
}