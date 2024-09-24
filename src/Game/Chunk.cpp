#include "Chunk.h"

int32_t FloatToIntNoise(float val)
{
	return static_cast<int32_t>(((val + 1) / 2) * 40);
}

Chunk::Chunk(glm::ivec2 position, FastNoiseLite noise, bool shouldBuild)
	:m_Blocks{0}
{
	this->m_Noise = noise;
	transform.position = { position.x, 0, position.y };
	transform.model = glm::translate(transform.model, glm::vec3(position.x, 0, position.y));

	m_Shader.AttachShader({ "res/shaders/cube.frag", GL_FRAGMENT_SHADER });
	m_Shader.AttachShader({ "res/shaders/cube.vert", GL_VERTEX_SHADER });

	m_GrassTex.Bind();
	m_Shader.setI("tex", 0);

	xCoord.reserve(CHUNK_SIZE);

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		xCoord.push_back(x);
	}

	glGenVertexArrays(1, &m_VAO);

	if (!shouldBuild) return;

	for (int32_t x = 0; x < CHUNK_SIZE; x++)
	{
		float noiseX = (position.x + static_cast<float>(x));
		for (int32_t z = 0; z < CHUNK_SIZE; z++)
		{
			float noiseY = (position.y + static_cast<float>(z));
			float noiseValue = m_Noise.GetNoise(noiseX, noiseY) / 2.0f + 0.5f;

			int32_t height = FloatToIntNoise(noiseValue);
			for (int32_t y = 0; y < height; y++)
			{
				if (OutOfBounds(x, y, z)) break;
				m_Blocks[GetBlockIndex(x, y, z)] = true;
			}
		}
	}

	for (int32_t x = 0; x < CHUNK_SIZE; x++)
	{
		float noiseX = (position.x + static_cast<float>(x));
		for (int32_t z = 0; z < CHUNK_SIZE; z++)
		{
			float noiseY = (position.y + static_cast<float>(z));
			float noiseValue = m_Noise.GetNoise(noiseX, noiseY) / 2.0f + 0.5f;

			int32_t height = FloatToIntNoise(noiseValue);
			for (int32_t y = 0; y < height; y++)
			{
				if (OutOfBounds(x, y, z)) break;
				AddBlock(x, y, z);
			}
		}
	}

	glBindVertexArray(m_VAO);

	m_IBO.uploadData(&m_Indices[0], m_Indices.size() * sizeof(unsigned int));
	m_VBO.uploadData(&m_Vertices[0], m_Vertices.size() * sizeof(float));

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

Chunk::~Chunk()
{
	if (m_ChunkThread.joinable())
		m_ChunkThread.join();

	m_Vertices.clear();
	m_Indices.clear();
	glDeleteVertexArrays(1, &m_VAO);
}

void Chunk::CleanChunk()
{
	if (m_ChunkThread.joinable())
		m_ChunkThread.join();

	m_Blocks.fill(0);
	m_Vertices.clear();
	m_Indices.clear();

	isCleaned = true;
	isChunkLoaded = false;
}

void Chunk::GenerateChunkT(glm::ivec2 position, FastNoiseLite& noise)
{
	this->m_Noise = noise;
	transform.model = glm::translate(transform.model, glm::vec3(position.x, 0, position.y));
	transform.position = { position.x, 0, position.y };
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
			float noiseValue = m_Noise.GetNoise(noiseX, noiseY);

			int32_t height = FloatToIntNoise(noiseValue);
			for (int32_t y = 0; y < height; y++)
			{
				if (OutOfBounds(x) || OutOfBounds(z) || OutOfBounds(y, CHUNK_HEIGHT)) break;
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
			float noiseValue = m_Noise.GetNoise(noiseX, noiseY);

			int32_t height = FloatToIntNoise(noiseValue);
			for (int32_t y = 0; y < height; y++)
			{
				if (OutOfBounds(x) || OutOfBounds(z) || OutOfBounds(y, CHUNK_HEIGHT)) break;
				AddBlock(x, y, z);
			}
		}
	}

	glBindVertexArray(m_VAO);

	m_IBO.uploadData(&m_Indices[0], m_Indices.size() * sizeof(unsigned int));
	m_VBO.uploadData(&m_Vertices[0], m_Vertices.size() * sizeof(float));

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	isChunkLoaded = true;
	isCleaned = false;
}

int Chunk::GetBlockIndex(const int32_t x, const int y, const int32_t z) const
{
	return y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x;
}

bool Chunk::GetBlock(const int x, const int y, const int z) const
{
	if (x < 0 || x >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE) return false;
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
	/*if (m_Blocks[GetBlockIndex(x, y, z - 1)]
		&& m_Blocks[GetBlockIndex(x, y, z + 1)]
		&& m_Blocks[GetBlockIndex(x + 1, y, z)]
		&& m_Blocks[GetBlockIndex(x - 1, y, z)]
		&& m_Blocks[GetBlockIndex(x, y + 1, z)]
		&& m_Blocks[GetBlockIndex(x, y - 1, z)])
	{
		return;
	}*/

	std::array<float, 20> frontFace = {
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f
	};
	std::array<float, 20> backFace = {
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f
	};
	std::array<float, 20> leftFace = {
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f
	};
	std::array<float, 20> rightFace = {
		0.5f,  0.5f,  0.5f,		1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,		1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		1.0f, 0.0f
	};
	std::array<float, 20> bottomFace = {
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		1.0f, 0.0f
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f
	};
	std::array<float, 20> topFace = {
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,		1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,		1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,
	};

	glm::ivec3 facePos = { x,y,z };
	TryAddFace(frontFace, facePos, { x, y, z - 1 });
	TryAddFace(backFace, facePos, { x, y, z + 1 });
	TryAddFace(leftFace, facePos, { x - 1, y, z });
	TryAddFace(rightFace, facePos, { x + 1, y, z });
	if (y != 0)
		TryAddFace(bottomFace, facePos, { x, y - 1, z });
	TryAddFace(topFace, facePos, { x, y + 1, z});
}

void Chunk::TryAddFace(const std::array<float, 20>& blockFace, const glm::ivec3& facePos, const glm::ivec3& adjacentBlock)
{
	if (OutOfBounds(adjacentBlock.x, adjacentBlock.y, adjacentBlock.z))
	{
		AddFace(blockFace, facePos);
	}
	else if (!m_Blocks[GetBlockIndex(adjacentBlock.x, adjacentBlock.y, adjacentBlock.z)])
	{
		AddFace(blockFace, facePos);
	}
}

void Chunk::AddFace(const std::array<float, 20> &blockFace, const glm::ivec3 &facePos)
{
	int nbElementPerLayout = 5;
	int countVertices = m_Vertices.size() / nbElementPerLayout;
	for (int i = 0; i < blockFace.size(); i+=5)
	{
		m_Vertices.push_back(facePos.x + blockFace[i]);
		m_Vertices.push_back(facePos.y + blockFace[i+1]);
		m_Vertices.push_back(facePos.z + blockFace[i+2]);
		m_Vertices.push_back(blockFace[i + 3]);	// UV
		m_Vertices.push_back(blockFace[i + 4]);	// UV
	}

	m_Indices.push_back(countVertices);
	m_Indices.push_back(countVertices + 1);
	m_Indices.push_back(countVertices + 2);
	m_Indices.push_back(countVertices + 2);
	m_Indices.push_back(countVertices + 3);
	m_Indices.push_back(countVertices);
}

void Chunk::Draw()
{
	m_Shader.Bind();
	m_Shader.setMat4("model", transform.model);
	m_Shader.setMat4("view", transform.view);
	m_Shader.setMat4("proj", transform.proj);

	//std::cout << transform.position.x << "\n";

	glBindVertexArray(m_VAO);
	m_IBO.Bind();
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	m_IBO.UnBind();
	glBindVertexArray(0);

	m_Shader.UnBind();
}