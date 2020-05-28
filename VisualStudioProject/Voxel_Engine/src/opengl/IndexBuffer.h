#pragma once

class IndexBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	enum class DrawType
	{
		STATIC,
		DYNAMIC,
		STREAM
	};

	IndexBuffer();
	IndexBuffer(const unsigned int* data, unsigned int count, DrawType type = DrawType::STATIC);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	void Create(const void* data, const unsigned int& count, DrawType type = DrawType::STATIC) const;
	void SetData(const unsigned int* data, const unsigned int& count, const unsigned int& Position_in_Memory) const;

	inline unsigned int GetCount() const { return m_Count; }
};

