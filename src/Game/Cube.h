#pragma once

#include "../OpenGL/Shader.h"
#include "../OpenGL/Buffer.h"

#include <iostream>

struct Transform
{
	glm::mat4 proj{ 1.f };
	glm::mat4 view{ 1.f };
	glm::mat4 model{ 1.f };
};

class Cube
{
public:
	Cube();
	~Cube();

	Cube(const Cube&) = default;
	Cube& operator=(const Cube&) = default;
	Cube(Cube&&) = default;
	Cube& operator=(Cube&&) = default;

	void Draw();
	Transform transform{};

private:
	Shader m_Shader{};
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
	Buffer m_VBO{{GL_ARRAY_BUFFER, vertices, sizeof(vertices), GL_STATIC_DRAW}};
	unsigned int m_VAO=0;
};