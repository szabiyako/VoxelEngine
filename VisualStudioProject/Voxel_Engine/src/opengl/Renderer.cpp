#include "Renderer.h"

#include <iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "): " << function <<
			' ' << file << ':' << line << std::endl;
		return false;
	}
	return true;
}

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT); //now can use this
}

void Renderer::Draw(const VertexArray & va, const IndexBuffer & ib, const Shader & shader, bool bindALL /* = true*/) const
{
	if (bindALL)
	{
		shader.Bind();
		va.Bind();
		ib.Bind();
	}

	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(const VertexArray & va, const IndexBuffer & ib, const Shader & shader, const Texture & texture, bool bindALL /* = true*/) const
{
	if (bindALL)
	{
		shader.Bind();
		va.Bind();
		ib.Bind();
		texture.Bind();
	}

	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::DrawInstanced(const VertexArray & va, const IndexBuffer & ib, const unsigned int & count, const Shader & shader, const Texture & texture, bool bindALL) const
{
	if (bindALL)
	{
		shader.Bind();
		va.Bind();
		ib.Bind();
		texture.Bind();
	}

	GLCall(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
}
