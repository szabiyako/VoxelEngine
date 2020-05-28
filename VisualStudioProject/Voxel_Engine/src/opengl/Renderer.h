#pragma once

#include <GL/glew.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"


#define ASSERT(x) if (!(x)) __debugbreak();

#ifdef DEBUGMODE
	#define GLCall(x) GLClearError();\
		x;\
		ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
	#define GLCall(x) x;
#endif


void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
private:

public:
	void Clear() const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, bool bindALL = true) const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, const Texture& texture, bool bindALL = true) const;
	void DrawInstanced(const VertexArray& va, const IndexBuffer& ib, const unsigned int& count, const Shader& shader, const Texture& texture, bool bindALL = true) const;
};


