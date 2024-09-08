#pragma once

#include "GameObject.h"
#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexBuffer.h"

#include <iostream>

class Cube
{
public:
	struct Transform
	{
		glm::mat4 proj{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 model{ 1.f };
	};

	Cube();
	~Cube();

	void Draw();

	Transform transform{};

private:
	Shader m_Shader{ "res/shaders/cube.frag", "res/shaders/cube.vert" };
	float vertices[36*8]{
		// Positions			// Normals				// UV
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,	  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,	  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,	0.0f, 0.0f, -1.0f,	  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	0.0f, 0.0f, -1.0f,	  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, -1.0f,	  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,	  0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,  1.0f,	  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, 0.0f,  1.0f,	  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	0.0f, 0.0f,  1.0f,	  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	0.0f, 0.0f,  1.0f,	  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 0.0f,  1.0f,	  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,  1.0f,	  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,   -1.0f, 0.0f,  0.0f,	  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,   -1.0f, 0.0f,  0.0f,	  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,   -1.0f, 0.0f,  0.0f,	  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,   -1.0f, 0.0f,  0.0f,	  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,   -1.0f, 0.0f,  0.0f,	  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,   -1.0f, 0.0f,  0.0f,	  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,  0.0f,	  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 0.0f,  0.0f,	  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,  0.0f,	  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,  0.0f,	  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,  0.0f,	  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,  0.0f,	  1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,  0.0f,	  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	0.0f, 1.0f,  0.0f,	  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, 1.0f,  0.0f,	  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, 1.0f,  0.0f,	  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 1.0f,  0.0f,	  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,  0.0f,	  0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,  0.0f,	  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	0.0f, 1.0f,  0.0f,	  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	0.0f, 1.0f,  0.0f,	  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	0.0f, 1.0f,  0.0f,	  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,  0.0f,	  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,  0.0f,	  0.0f, 1.0f
	};
	VertexBuffer m_VBO{vertices, sizeof(vertices)};
	unsigned int m_VAO;
};