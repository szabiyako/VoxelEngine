#include "VertexBuffer.h"

#include "Renderer.h"

VertexBuffer::VertexBuffer()
{
	glGenBuffers(1, &this->m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_RendererID);
}

VertexBuffer::VertexBuffer(const void * data, unsigned int size, DrawType type /*= DrawType::STATIC*/)
{
	glGenBuffers(1, &this->m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_RendererID);

	if (type == DrawType::STATIC)
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	else if (type == DrawType::DYNAMIC)
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	else if (type == DrawType::STREAM)
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STREAM_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &this->m_RendererID);
}

void VertexBuffer::Create(const void * data, const unsigned int& size, DrawType type /*= DrawType::STATIC*/) const
{
	glBindBuffer(GL_ARRAY_BUFFER, this->m_RendererID);

	if (type == DrawType::STATIC)
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	else if (type == DrawType::DYNAMIC)
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	else if (type == DrawType::STREAM)
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STREAM_DRAW);
}

void VertexBuffer::SetData(const void * data, const unsigned int & size_in_bytes, const unsigned int & Position_in_Memory) const
{
	glBindBuffer(GL_ARRAY_BUFFER, this->m_RendererID);
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, Position_in_Memory, size_in_bytes, data));
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, this->m_RendererID);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
