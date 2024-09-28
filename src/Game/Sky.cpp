#include "Sky.h"

Sky::Sky()
{
	m_Shader.AttachShader({"res/shaders/sky.frag", GL_FRAGMENT_SHADER});
	m_Shader.AttachShader({"res/shaders/sky.vert", GL_VERTEX_SHADER});

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	m_VBO.UploadData(&m_Vertices[0], m_Vertices.size() * sizeof(Vertex));
}

Sky::~Sky()
{
	glDeleteBuffers(1, &m_VAO);

	m_Vertices.clear();
	m_Shader.DestroyShader();
	m_VBO.DestroyBuffer();
}

void Sky::CreateHemisphere(int numRows, int numCols, float radius)
{
	int numVerticesTopStrip = 3 * numCols;
	int numVerticesRegularStrip = 6 * numCols;

	m_Vertices.reserve(numVerticesTopStrip + (numRows - 1) * numVerticesRegularStrip);

	float pitchAngle = 90.f / (float)numRows;
	float headingAngle = 360.f / (float)numCols;

	glm::vec3 apex(0.f, radius, 0.f);

	float pitch = -90.f;

	for (float heading = 0.0f; heading < 360.f; heading += headingAngle)
	{
	}
}

void Sky::Draw(glm::vec3 viewPos)
{
	m_Shader.Bind();
	m_Shader.setMat4("proj", transform.proj);
	m_Shader.setMat4("view", transform.view);
	m_Shader.setMat4("model", transform.model);
	m_Shader.setVec3("viewPos", viewPos);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, m_Vertices.size());
	glBindVertexArray(0);

	m_Shader.UnBind();
}