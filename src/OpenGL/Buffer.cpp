#include "Buffer.h"

#include <iostream>

Buffer::Buffer(CreateInfo const& createInfo)
	:m_Target(createInfo.target), m_Usage(createInfo.usage)
{
	glGenBuffers(1, &m_BufferID);
	glBindBuffer(createInfo.target, m_BufferID);
	glBufferData(createInfo.target, createInfo.size, createInfo.data, createInfo.usage);
}

Buffer::~Buffer() noexcept
{
	DestroyBuffer();
}

void Buffer::UploadData(const void* data, size_t size)
{
	Bind();
	glBufferData(m_Target, size, data, m_Usage);
}

void Buffer::DestroyBuffer()
{
	glDeleteBuffers(1, &m_BufferID);
}

void Buffer::Bind() const
{
	glBindBuffer(m_Target, m_BufferID);
}

void Buffer::UnBind() const
{
	glBindBuffer(m_Target, 0);
}