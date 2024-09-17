#include "Chunk.h"

int FloatToIntNoise(float val)
{
	if (val >= 1) val = 0.999f;
	return 10 + val * 10;
}

Chunk::Chunk(glm::ivec2 position, const std::array<float, CHUNK_SIZE*CHUNK_SIZE> &noiseOutput)
	:m_Blocks{0}
{
	transform.model = glm::translate(transform.model, glm::vec3(position.x, 0, position.y));

	m_Shader.AttachShader({ "res/shaders/cube.frag", GL_FRAGMENT_SHADER });
	m_Shader.AttachShader({ "res/shaders/cube.vert", GL_VERTEX_SHADER });

	grassTex.Bind();
	m_Shader.setI("tex", 0);

	glGenVertexArrays(1, &m_VAO);

	int index = 0;
	for (int8_t x = 0; x < CHUNK_SIZE; x++)
	{
		for (int8_t z = 0; z < CHUNK_SIZE; z++)
		{
			int height = FloatToIntNoise(noiseOutput[index++]);
			for (int y = 0; y < height; y++)
			{
				if (OutOfBounds(x, y, z)) return;
				m_Blocks[GetBlockIndex(x, y, z)] = true;
			}
		}
	}

	index = 0;
	for (int8_t x = 0; x < CHUNK_SIZE; x++)
	{
		for (int8_t z = 0; z < CHUNK_SIZE; z++)
		{
			int height = FloatToIntNoise(noiseOutput[index++]);
			for (int y = 0; y < height; y++)
			{
				if (OutOfBounds(x, y, z)) return;
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
	glDeleteVertexArrays(1, &m_VAO);
}

void Chunk::CleanChunk()
{
	isCleaned = true;
	isChunkLoaded = false;
	std::memset(m_Blocks, 0, sizeof(m_Blocks));
	m_Vertices.clear();
	m_Indices.clear();
}

void Chunk::BuildChunk(glm::ivec2 position, const std::array<float, CHUNK_SIZE* CHUNK_SIZE>& noiseOutput)
{
	isCleaned = false;
	isChunkLoaded = true;
	transform.model = glm::translate(transform.model, glm::vec3(position.x, 0, position.y));

	int index = 0;
	for (int8_t x = 0; x < CHUNK_SIZE; x++)
	{
		for (int8_t z = 0; z < CHUNK_SIZE; z++)
		{
			int height = FloatToIntNoise(noiseOutput[index++]);
			for (int y = 0; y < height; y++)
			{
				if (OutOfBounds(x, y, z)) return;
				m_Blocks[GetBlockIndex(x, y, z)] = true;
			}
		}
	}

	index = 0;
	for (int8_t x = 0; x < CHUNK_SIZE; x++)
	{
		for (int8_t z = 0; z < CHUNK_SIZE; z++)
		{
			int height = FloatToIntNoise(noiseOutput[index++]);
			for (int y = 0; y < height; y++)
			{
				if (OutOfBounds(x, y, z)) return;
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

int Chunk::GetBlockIndex(const int8_t x, const int y, const int8_t z) const
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
	return OutOfBounds(x) || OutOfBounds(y) || OutOfBounds(z);
}

bool Chunk::OutOfBounds(const int value)
{
	return value < 0 || value >= CHUNK_SIZE;
}

void Chunk::AddBlock(const int8_t x, const int y, const int8_t z)
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

	glBindVertexArray(m_VAO);
	m_IBO.Bind();
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	m_IBO.UnBind();
	glBindVertexArray(0);

	m_Shader.UnBind();
}