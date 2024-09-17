#include "World.h"

#include <chrono>

World::World()
{
	const auto start = std::chrono::steady_clock::now();

	auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
	auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();

	fnFractal->SetSource(fnSimplex);
	fnFractal->SetOctaveCount(5);

	for (int16_t x = -chunkDiameter; x <= chunkDiameter; x += CHUNK_SIZE)
	{
		for (int16_t z = -chunkDiameter; z <= chunkDiameter; z += CHUNK_SIZE)
		{
			fnFractal->GenUniformGrid3D(noiseOutput.data(), x * CHUNK_SIZE, 0, z * CHUNK_SIZE, CHUNK_SIZE, 1, CHUNK_SIZE, 0.025f, seed);

			for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
			{
				noiseOutput[i] += 1;
				noiseOutput[i] /= 2;
				noiseOutput[i] = powf(noiseOutput[i], 2.f);
			}

			chunks[{x, z}].BuildChunk({ x,z }, noiseOutput);
		}
	}

	const std::chrono::duration<double> timeTaken{ std::chrono::steady_clock::now() - start };
	std::cout << "Time to build chunk: " << timeTaken.count() << "s\n";
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
		if (chunk.second.isChunkLoaded)
		{
			chunk.second.transform.proj = cam.getProjMatrix();
			chunk.second.transform.view = cam.getViewMatrix();
			chunk.second.Draw();
		}

		if (glm::abs(glm::distance(glm::vec3(chunk.first.x, 0, chunk.first.y), glm::vec3(cam.getPosition().x, 0, cam.getPosition().z))) > unloadDistance)
		{
			chunk.second.CleanChunk();
		}
	}
}