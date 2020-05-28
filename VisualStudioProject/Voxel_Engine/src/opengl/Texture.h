#pragma once

#include <string>

//
#include "glm/glm.hpp"

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char *m_LocalBuffer;
	int m_Width;
	int m_Height;
	int m_BPP; //bits per pixel
public:
	Texture(const std::string& filepath);

	//OnecoloredTexture
	Texture(const unsigned char& r = 255, const unsigned char& g = 255, const unsigned char& b = 255);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	void SetAnisotropicLevel(float level);
	static float GetMaxAnisotropicLevel();

	//temp
	inline unsigned int GetID() const { return m_RendererID; }
	inline unsigned int GetWidth() const { return m_Width; }
	inline unsigned int GetHeight() const { return m_Height; }

	static float** GetNoise();
};

