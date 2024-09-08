#include "Cube.h"

Cube::Cube()
{
	m_Shader.AttachShader({"res/shaders/cube.frag", GL_FRAGMENT_SHADER});
	m_Shader.AttachShader({"res/shaders/cube.vert", GL_VERTEX_SHADER});

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	m_VBO.Bind();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // uvs
	glEnableVertexAttribArray(2);
}

Cube::~Cube()
{
	glDeleteBuffers(1, &m_VAO);
}

void Cube::Draw()
{
	m_Shader.Bind();
	m_Shader.setMat4("proj", transform.proj);
	m_Shader.setMat4("view", transform.view);
	m_Shader.setMat4("model", transform.model);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	m_Shader.UnBind();
}