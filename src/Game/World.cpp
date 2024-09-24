#include "World.h"

World::World()
{
	noise.SetSeed(seed);
	noise.SetFractalOctaves(5);
	noise.SetFractalLacunarity(1.75f);
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);

	for (int16_t x = -chunkDiameter; x <= chunkDiameter; x += CHUNK_SIZE)
	{
		for (int16_t z = -chunkDiameter; z <= chunkDiameter; z += CHUNK_SIZE)
		{
			chunks[{x, z}].GenerateChunkT({ x,z }, noise);
		}
	}
}

bool World::GetBlock(const int x, const int y, const int z)
{
	glm::ivec2 blockPos = {
		x % CHUNK_SIZE,
		z % CHUNK_SIZE
	};

	glm::ivec2 chunkPos = {
		x / CHUNK_SIZE,
		z / CHUNK_SIZE
	};

	return chunks[{chunkPos.x * CHUNK_SIZE, chunkPos.y * CHUNK_SIZE}].GetBlock(blockPos.x, y, blockPos.y);
}

void World::Draw(const Camera& cam)
{
	float unloadDistance = static_cast<float>(m_RenderDistance + 1) * CHUNK_SIZE + 8.0f;
	for (auto &chunk : chunks)
	{
		// Chunk Drawing
		if (chunk.second.isChunkLoaded)
		{
			chunk.second.transform.proj = cam.getProjMatrix();
			chunk.second.transform.view = cam.getViewMatrix();
			chunk.second.transform.camPos = cam.getPosition();
			chunk.second.Draw();
		}

		// Chunk Cleaning
		if (glm::abs(glm::distance(glm::vec3(chunk.first.x, 0, chunk.first.y), glm::vec3(cam.getPosition().x, 0, cam.getPosition().z))) > unloadDistance && chunk.second.isChunkLoaded)
		{
			chunk.second.CleanChunk();
		}

		// Chunk Loading
		int cameraX = cam.getPosition().x / CHUNK_SIZE;
		int cameraZ = cam.getPosition().z / CHUNK_SIZE;

		bool isChunkLoaded = false;
		for (int i = 0; i < m_LoadDistance; i++)
		{
			int minX = cameraX - i;
			int minZ = cameraZ - i;
			int maxX = cameraX + i;
			int maxZ = cameraZ + i;

			for (int x = minX; x < maxX; x++)
			{
				for (int z = minZ; z < maxZ; z++)
				{
					if (x != minX && x != maxX - 1 && z != minZ && z != maxZ - 1 && !chunks[{x*CHUNK_SIZE, z*CHUNK_SIZE}].isChunkLoaded)
					{
						chunks[{x* CHUNK_SIZE, z* CHUNK_SIZE}].GenerateChunkT({ x * CHUNK_SIZE, z * CHUNK_SIZE }, noise);
						isChunkLoaded = chunks[{x * CHUNK_SIZE, z * CHUNK_SIZE}].isChunkLoaded;
					}
				}
			}

			if (isChunkLoaded) break;
		}

		if (!isChunkLoaded) m_LoadDistance++;
		if (m_LoadDistance >= m_RenderDistance) m_LoadDistance = 2;
	}
}