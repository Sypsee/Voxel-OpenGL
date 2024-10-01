#pragma once

#include "Window.h"
#include "Camera.h"
#include "Renderer.h"
#include "../Game/World.h"
#include "../Game/Clouds.h"
#include "../Game/Sky.h"

#include <unordered_map>
#include <array>

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
	World *world;
	Sky *sky;
	//Cloud* cloud;
};