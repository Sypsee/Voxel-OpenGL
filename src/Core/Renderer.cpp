#include "Renderer.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::Draw(std::vector<Cube> &cubes)
{
	for (auto &obj : cubes)
	{
		obj.Draw();
	}
}