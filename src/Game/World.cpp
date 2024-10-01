#include "World.h"

#include "FrustumCulling.h"

World::World()
{
	noise.SetSeed(seed);
	noise.SetFractalOctaves(5);
	noise.SetFractalLacunarity(1.75f);
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
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
	fc.Update(cam);

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
				int chunkXPos = x * CHUNK_SIZE;
				int chunkZPos = z * CHUNK_SIZE;

				if (x != minX && x != maxX - 1 && z != minZ && z != maxZ - 1 && !chunks[{chunkXPos, chunkZPos}].isChunkLoaded)
				{
					chunks[{chunkXPos, chunkZPos}].GenerateChunkT({ chunkXPos, chunkZPos }, noise);
					isChunkLoaded = chunks[{chunkXPos, chunkZPos}].isChunkLoaded;
				}
			}
		}

		if (isChunkLoaded) break;
	}

	if (!isChunkLoaded) m_LoadDistance++;
	if (m_LoadDistance >= m_RenderDistance) m_LoadDistance = 2;

	float unloadDistance = static_cast<float>(m_RenderDistance + 1) * CHUNK_SIZE;
	for (auto chunk = chunks.begin(); chunk != chunks.end();)
	{
		// Chunk Drawing
		if (chunks.find(chunk->first) != chunks.end() && chunk->second.aabb.isOnFrustum(fc.frustum, chunk->second.transform))
		{
			chunk->second.transform.proj = cam.getProjMatrix();
			chunk->second.transform.view = cam.getViewMatrix();
			chunk->second.transform.camPos = cam.getPosition();
			chunk->second.Draw();
		}

		// Chunk Cleaning
		if (glm::abs(glm::distance(glm::vec3(chunk->first.x, 0, chunk->first.y), glm::vec3(cam.getPosition().x, 0, cam.getPosition().z))) > unloadDistance && chunk->second.isChunkLoaded)
		{
			chunk = chunks.erase(chunk);
		}
		else
		{
			chunk++;
		}
	}
}