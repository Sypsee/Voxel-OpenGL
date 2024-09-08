#pragma once

#include "Window.h"
#include "Camera.h"
#include "Renderer.h"
#include "../Game/GameObject.h"
#include "../Game/Cube.h"

#include <vector>

class Application
{
public:
	static constexpr int START_WIDTH = 800;
	static constexpr int START_HEIGHT = 600;

	Application();
	~Application();

	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	void run();

private:
	Window m_Window{ "Voxel Engine", START_WIDTH, START_HEIGHT };
	Renderer renderer{};
	std::vector<Cube> cubes;
};