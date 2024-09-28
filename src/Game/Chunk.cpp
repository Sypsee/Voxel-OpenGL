#include "Chunk.h"

#include <cmath>

int32_t FloatToIntNoise(float val)
{
	return static_cast<int32_t>(((val + 1) / 2) * 90);
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

	m_ChunkThread = std::thread(&Chunk::BuildChunk, this);
	//BuildChunk();
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
	std::array<Vertex, 6> frontFace = {
		Vertex{
			glm::i8vec3(0, 0, 0),
			0,
			0
		},
		Vertex{
			glm::i8vec3(1, 0, 0),
			1,
			0
		},
		Vertex{
			glm::i8vec3(1, 1, 0),
			3,
			0
		},
		Vertex{
			glm::i8vec3(1, 1, 0),
			3,
			0
		},
		Vertex{
			glm::i8vec3(0, 1, 0),
			2,
			0
		},
		Vertex{
			glm::i8vec3(0, 0, 0),
			0,
			0
		}
	};

	std::array<Vertex, 6> backFace = {
		Vertex{
			glm::i8vec3(0, 0,  1),
			0,
			1
		},
		Vertex{
			glm::i8vec3(1, 0, 1),
			1,
			1
		},
		Vertex{
			glm::i8vec3(1, 1, 1),
			3,
			1
		},
		Vertex{
			glm::i8vec3(1, 1, 1),
			3,
			1
		},
		Vertex{
			glm::i8vec3(0, 1, 1),
			2,
			1
		},
		Vertex{
			glm::i8vec3(0, 0, 1),
			0,
			1
		}
	};

	std::array<Vertex, 6> leftFace = {
		Vertex{
			glm::i8vec3(0,  1,  1),
			1,
			2
		},
		Vertex{
			glm::i8vec3(0, 1, 0),
			3,
			2
		},
		Vertex{
			glm::i8vec3(0, 0, 0),
			2,
			2
		},
		Vertex{
			glm::i8vec3(0, 0, 0),
			2,
			2
		},
		Vertex{
			glm::i8vec3(0, 0, 1),
			0,
			2
		},
		Vertex{
			glm::i8vec3(0, 1, 1),
			1,
			2
		}
	};

	std::array<Vertex, 6> rightFace = {
		Vertex{
			glm::i8vec3(1,  1,  1),
			1,
			3
		},
		Vertex{
			glm::i8vec3(1, 1, 0),
			3,
			3
		},
		Vertex{
			glm::i8vec3(1, 0, 0),
			2,
			3
		},
		Vertex{
			glm::i8vec3(1, 0, 0),
			2,
			3
		},
		Vertex{
			glm::i8vec3(1, 0, 1),
			0,
			3
		},
		Vertex{
			glm::i8vec3(1, 1, 1),
			1,
			3
		}
	};

	std::array<Vertex, 6> bottomFace = {
		Vertex{
			glm::i8vec3(0,  0,  0),
			2,
			4
		},
		Vertex{
			glm::i8vec3(1, 0, 0),
			3,
			4
		},
		Vertex{
			glm::i8vec3(1, 0, 1),
			1,
			4
		},
		Vertex{
			glm::i8vec3(1, 0, 1),
			1,
			4
		},
		Vertex{
			glm::i8vec3(0, 0, 1),
			0,
			4
		},
		Vertex{
			glm::i8vec3(0, 0, 0),
			2,
			4
		}
	};

	std::array<Vertex, 6> topFace = {
		Vertex{
			glm::i8vec3(0,  1, 0),
			2,
			9
		},
		Vertex{
			glm::i8vec3(1, 1, 0),
			3,
			9
		},
		Vertex{
			glm::i8vec3(1, 1, 1),
			1,
			9
		},
		Vertex{
			glm::i8vec3(1, 1, 1),
			1,
			9
		},
		Vertex{
			glm::i8vec3(0, 1, 1),
			0,
			9
		},
		Vertex{
			glm::i8vec3(0, 1, 0),
			2,
			9
		}
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
		/*if (cXN != nullptr)
		{
			if (cXN->GetBlock(adjacentBlock.x, adjacentBlock.y, adjacentBlock.z)) return;
		}
		if (cXP != nullptr)
		{
			if (cXP->GetBlock(adjacentBlock.x, adjacentBlock.y, adjacentBlock.z)) return;
		}
		if (cZN != nullptr)
		{
			if (cZN->GetBlock(adjacentBlock.x, adjacentBlock.y, adjacentBlock.z)) return;
		}
		if (cZP != nullptr)
		{
			if (cZP->GetBlock(adjacentBlock.x, adjacentBlock.y, adjacentBlock.z)) return;
		}*/

		AddFace(blockFace, facePos);
	}
}

void Chunk::AddFace(const std::array<Vertex, 6> &blockFace, const glm::ivec3 &facePos)
{
	try {
		std::lock_guard<std::mutex> lock(mtx);

		for (const Vertex& vert : blockFace) {
			m_Vertices.push_back(
				Vertex{
					{
						static_cast<int8_t>(facePos.x) + vert.position.x,
						static_cast<int8_t>(facePos.y) + vert.position.y,
						static_cast<int8_t>(facePos.z) + vert.position.z,
					},
					vert.uvIndex,
					vert.normalIndex
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

		glVertexAttribIPointer(0, 3, GL_BYTE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::uvIndex)));
		glEnableVertexAttribArray(1);
		glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::normalIndex)));
		glEnableVertexAttribArray(2);

		generateGLData = false;
	}

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