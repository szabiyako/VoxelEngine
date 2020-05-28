#include "IndexBuffer.h"

#include "Renderer.h"

IndexBuffer::IndexBuffer()
{
	ASSERT(sizeof(unsigned int) == sizeof(GLuint));

	GLCall(glGenBuffers(1, &this->m_RendererID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_RendererID));
}

IndexBuffer::IndexBuffer(const unsigned int * data, unsigned int count, DrawType type /*= DrawType::STATIC*/)
	: m_Count(count)
{
	ASSERT(sizeof(unsigned int) == sizeof(GLuint));

	GLCall(glGenBuffers(1, &this->m_RendererID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_RendererID));
	if (type == DrawType::STATIC)
	{
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
	}
	else if (type == DrawType::DYNAMIC)
	{
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_DYNAMIC_DRAW));
	}
	else if (type == DrawType::STREAM)
	{
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STREAM_DRAW));
	}
}

IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1, &this->m_RendererID));
}

void IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_RendererID));
}

void IndexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void IndexBuffer::Create(const void * data, const unsigned int & count, DrawType type) const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_RendererID));
	if (type == DrawType::STATIC)
	{
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
	}
	else if (type == DrawType::DYNAMIC)
	{
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_DYNAMIC_DRAW));
	}
	else if (type == DrawType::STREAM)
	{
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STREAM_DRAW));
	}
}

void IndexBuffer::SetData(const unsigned int * data, const unsigned int & count, const unsigned int & Position_in_Memory) const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_RendererID));
	GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, Position_in_Memory, count * sizeof(unsigned int), data));
}
