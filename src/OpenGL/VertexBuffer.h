#pragma once

#include <glad/glad.h>


class VertexBuffer
{
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void UnBind() const;

private:
	unsigned int m_BufferID=0;
	const void* m_Data;
};