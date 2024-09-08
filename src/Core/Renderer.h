#pragma once

#include <vector>

#include "../Game/Cube.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Draw(std::vector<Cube> &cubes);
};