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
			float noiseValue = m_Noise.GetNoise(noiseX, noiseY) / 2.0f + 1;

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
			float noiseValue = m_Noise.GetNoise(noiseX, noiseY) / 2.0f + 1;

			int32_t height = FloatToIntNoise(noiseValue);
			for (int32_t y = 0; y < height; y++)
			{
				if (OutOfBounds(x, y, z)) break;
				AddBlock(x, y, z);
			}
		}
	}

	glBindVertexArray(m_VAO);

	//m_IBO.UploadData(&m_Indices[0], m_Indices.size() * sizeof(unsigned int));
	m_VBO.UploadData(&m_Vertices[0], m_Vertices.size() * sizeof(float));

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

Chunk::~Chunk()
{
	CleanChunk();
}

void Chunk::CleanChunk()
{
	if (m_ChunkThread.joinable())
		m_ChunkThread.join();

	m_Blocks.fill(0);
	m_Vertices.clear();
	m_Indices.clear();
	m_Noise = NULL;

	glDeleteVertexArrays(1, &m_VAO);
	m_Shader.DestroyShader();
	//m_VBO.DestroyBuffer();

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
			float noiseValue = m_Noise.GetNoise(noiseX, noiseY);

			int32_t height = FloatToIntNoise(noiseValue);
			for (int32_t y = 0; y < height; y++)
			{
				if (OutOfBounds(x, y, z)) break;
				AddBlock(x, y, z);
			}
		}
	}

	glBindVertexArray(m_VAO);

	m_VBO.UploadData(&m_Vertices[0], m_Vertices.size() * sizeof(Vertex));

	glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::uv)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 1, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::normalIndex)));
	glEnableVertexAttribArray(2);

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
	std::array<Vertex, 4> frontFace = {
		Vertex{
			glm::i8vec3(-1, -1, -1),
			glm::u8vec2(0, 0),
			0
		},
		Vertex{
			glm::i8vec3(1, -1, -1),
			glm::u8vec2(1, 0),
			0
		},
		Vertex{
			glm::i8vec3(1, 1, -1),
			glm::u8vec2(1, 1),
			0
		},
		Vertex{
			glm::i8vec3(-1, 1, -1),
			glm::u8vec2(0, 1),
			0
		},
	};

	std::array<Vertex, 4> backFace = {
		Vertex{
			glm::i8vec3(-1, -1,  1),
			glm::u8vec2(0, 0),
			1
		},
		Vertex{
			glm::i8vec3(1, -1, 1),
			glm::u8vec2(1, 0),
			1
		},
		Vertex{
			glm::i8vec3(1, 1, 1),
			glm::u8vec2(1, 1),
			1
		},
		Vertex{
			glm::i8vec3(-1, 1, 1),
			glm::u8vec2(0, 1),
			1
		},
	};

	std::array<Vertex, 4> leftFace = {
		Vertex{
			glm::i8vec3(-1,  1,  1),
			glm::u8vec2(0, 1),
			2
		},
		Vertex{
			glm::i8vec3(-1, 1, -1),
			glm::u8vec2(0, 1),
			2
		},
		Vertex{
			glm::i8vec3(-1, -1, -1),
			glm::u8vec2(0, 0),
			2
		},
		Vertex{
			glm::i8vec3(-1, -1, 1),
			glm::u8vec2(0, 0),
			2
		},
	};

	std::array<Vertex, 4> rightFace = {
		Vertex{
			glm::i8vec3(1,  1,  1),
			glm::u8vec2(1, 1),
			3
		},
		Vertex{
			glm::i8vec3(1, 1, -1),
			glm::u8vec2(1, 1),
			3
		},
		Vertex{
			glm::i8vec3(1, -1, -1),
			glm::u8vec2(1, 0),
			3
		},
		Vertex{
			glm::i8vec3(1, -1, 1),
			glm::u8vec2(1, 0),
			3
		},
	};

	std::array<Vertex, 4> bottomFace = {
		Vertex{
			glm::i8vec3(-1,  -1,  -1),
			glm::u8vec2(0, 0),
			4
		},
		Vertex{
			glm::i8vec3(1, -1, -1),
			glm::u8vec2(1, 0),
			4
		},
		Vertex{
			glm::i8vec3(1, -1, 1),
			glm::u8vec2(1, 0),
			4
		},
		Vertex{
			glm::i8vec3(-1, -1, 1),
			glm::u8vec2(0, 0),
			4
		},
	};

	std::array<Vertex, 4> topFace = {
		Vertex{
			glm::i8vec3(-1,  1,  -1),
			glm::u8vec2(0, 1),
			5
		},
		Vertex{
			glm::i8vec3(1, 1, -1),
			glm::u8vec2(1, 1),
			5
		},
		Vertex{
			glm::i8vec3(1, 1, 1),
			glm::u8vec2(1, 1),
			5
		},
		Vertex{
			glm::i8vec3(-1, 1, 1),
			glm::u8vec2(0, 1),
			5
		},
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

void Chunk::TryAddFace(const std::array<Vertex, 4>& blockFace, const glm::ivec3& facePos, const glm::ivec3& adjacentBlock)
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

void Chunk::AddFace(const std::array<Vertex, 4> &blockFace, const glm::ivec3 &facePos)
{
	for (Vertex vert : blockFace)
	{
		m_Vertices.push_back(
			Vertex{
				{
					(static_cast<int8_t>(facePos.x) + vert.position.x),
					(static_cast<int8_t>(facePos.y) + vert.position.y),
					(static_cast<int8_t>(facePos.z) + vert.position.z),
				},
				vert.uv,
				vert.normalIndex
			}
		);
	}
}

void Chunk::Draw()
{
	m_Shader.Bind();
	m_Shader.setMat4("model", transform.model);
	m_Shader.setMat4("view", transform.view);
	m_Shader.setMat4("proj", transform.proj);
	m_Shader.setVec3("viewPos", transform.camPos);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, m_Vertices.size());
	glBindVertexArray(0);

	m_Shader.UnBind();
}