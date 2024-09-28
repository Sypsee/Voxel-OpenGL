#pragma once

#include "../OpenGL/Shader.h"
#include "../OpenGL/Buffer.h"
#include "ObjectComponents.h"

#include <iostream>
#include <vector>

struct Vertex
{
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 normal{ 0.f, 0.f, 0.f };
};

class Sky
{
public:
	Sky();
	~Sky();
	
	void CreateHemisphere(int numRows, int numCols, float radius);

	void Draw(glm::vec3 viewPos);
	Transform transform{};

private:
	std::vector<Vertex> m_Vertices;

	Shader m_Shader;
	Buffer m_VBO{ {GL_ARRAY_BUFFER, NULL, m_Vertices.size() * sizeof(Vertex), GL_STATIC_DRAW}};
	unsigned int m_VAO=0;
};