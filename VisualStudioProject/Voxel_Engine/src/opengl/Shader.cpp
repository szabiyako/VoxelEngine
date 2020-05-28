#include "Shader.h"

#include "Renderer.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string & filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}


ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (std::getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		//allocate pointer on stack, not on dram
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile ";
		std::cout << (type == GL_VERTEX_SHADER ? "vertex" : "fragment");
		std::cout << " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);

		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int programm = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(programm, vs);
	glAttachShader(programm, fs);
	glLinkProgram(programm);
	glValidateProgram(programm);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return programm;
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

int Shader::GetUniformLocation(const std::string & name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' doesn`t exist!" << std::endl;
	m_UniformLocationCache[name] = location;
	return location;
}

//UNIFORMS

void Shader::SetUniform1i(const std::string & name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string & name, float value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}


void Shader::SetUniform4f(const std::string & name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformVec2f(const std::string & name, glm::vec2 vec)
{
	GLCall(glUniform2f(GetUniformLocation(name), vec[0], vec[1]));
}

void Shader::SetUniformVec4f(const std::string & name, glm::vec4 vec)
{
	GLCall(glUniform4f(GetUniformLocation(name), vec[0], vec[1], vec[2], vec[3]));
}

void Shader::SetUniformMatrix4f(const std::string & name, glm::mat4 mat4)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat4[0][0]));
}
