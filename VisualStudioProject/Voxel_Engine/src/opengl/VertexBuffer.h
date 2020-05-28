#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	enum class DrawType
	{
		STATIC,
		DYNAMIC,
		STREAM
	};
	VertexBuffer();
	VertexBuffer(const void* data, unsigned int size, DrawType type = DrawType::STATIC);
	~VertexBuffer();


	void Create(const void* data, const unsigned int& size, DrawType type = DrawType::STATIC) const;
	void SetData(const void* data, const unsigned int& size_in_bytes, const unsigned int& Position_in_Memory) const;

	void Bind() const;
	void Unbind() const;
};

