#pragma once

#include <glad/glad.h>
#include <utility>

class Buffer final
{
public:
	struct CreateInfo final
	{
		GLenum target = 0;
		void const* data = nullptr;
		size_t size = 0;
		GLenum usage = 0;
	};

	constexpr Buffer() noexcept = delete;
	Buffer(CreateInfo const &createInfo);
	Buffer(Buffer const&) = delete;
	Buffer& operator=(Buffer const&) = delete;
	Buffer& operator=(Buffer&& other) noexcept
	{
		std::swap(m_BufferID, other.m_BufferID);
		return *this;
	}
	inline Buffer(Buffer&& other) noexcept { *this = std::move(other); }
	~Buffer() noexcept;

	void Bind() const;
	void UnBind() const;

private:
	unsigned int m_BufferID = 0;
	GLenum m_Target = 0;
};