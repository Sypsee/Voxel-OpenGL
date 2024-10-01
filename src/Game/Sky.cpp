#include "Sky.h"

Sky::Sky()
{
	//transform.model = glm::scale(transform.model, { 10.f, 10.f, 10.f });

	m_Shader.AttachShader({ "res/shaders/sky.frag", GL_FRAGMENT_SHADER });
	m_Shader.AttachShader({ "res/shaders/sky.vert", GL_VERTEX_SHADER });

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	m_VBO.UploadData(vertices, sizeof(vertices));

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

Sky::~Sky()
{
	glDeleteBuffers(1, &m_VAO);
	m_Shader.DestroyShader();
	m_VBO.DestroyBuffer();
}

void Sky::Draw()
{
	m_Shader.Bind();
	glm::mat4 view = glm::inverse(transform.view);
	view[3] = glm::vec4(0.f, 0.f, 0.f, 1.f);

	float normalizedTime = static_cast<float>(fmod(glfwGetTime(), 10)) / 10;
	m_Shader.setF("time", normalizedTime/50);
	m_Shader.setMat4("inverseView", view * glm::inverse(transform.proj));
	//m_Shader.setMat4("model", transform.model);

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);

	m_Shader.UnBind();
}