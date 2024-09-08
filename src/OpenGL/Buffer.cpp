#include "Buffer.h"

#include <iostream>

Buffer::Buffer(CreateInfo const& createInfo)
	:m_Target(createInfo.target)
{
	glGenBuffers(1, &m_BufferID);
	glBindBuffer(createInfo.target, m_BufferID);
	glBufferData(createInfo.target, createInfo.size, createInfo.data, createInfo.usage);
}

Buffer::~Buffer() noexcept
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