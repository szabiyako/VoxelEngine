#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "Renderer.h"

Texture::Texture(const std::string & filepath)
	: m_RendererID(0), m_FilePath(filepath), m_LocalBuffer(nullptr),
	  m_Width(0), m_Height(0), m_BPP(0)
{
	stbi_set_flip_vertically_on_load(true);
	m_LocalBuffer = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_BPP, 4);
	ASSERT(m_LocalBuffer);

	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	//for pixilisation
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Default
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//For no repeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//For repeat
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//16xAnisfiltr
	SetAnisotropicLevel(GetMaxAnisotropicLevel());

	//                               | Type output                   | Type input   | Data size per channel
	//                          | level of detail 0 - max       | pixels in Border
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
}

//OnecoloredTexture
Texture::Texture(const unsigned char& r /*= 255*/, const unsigned char& g /*= 255*/, const unsigned char& b /*= 255*/)
	: m_RendererID(0), m_FilePath("None"), m_LocalBuffer(nullptr), m_Width(1), m_Height(1), m_BPP(8)
{
	unsigned char tmp[4] = { r, g, b, 255 };

	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	SetAnisotropicLevel(GetMaxAnisotropicLevel());

	//                          | level                         | pixels in Border
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_RendererID);
}

void Texture::Bind(unsigned int slot /*= 0*/) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetAnisotropicLevel(float level)
{
	Bind();
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
}

float Texture::GetMaxAnisotropicLevel()
{
	float fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	return fLargest;
}
float** Texture::GetNoise()
{
	stbi_set_flip_vertically_on_load(false);
	unsigned char *perlData;
	int perlWidth;
	int perlHeight;
	int perlBPP; //bits per pixel
	perlData = stbi_load("res/textures/600x200.png", &perlWidth, &perlHeight, &perlBPP, 3);
	struct VoxelColor
	{
		unsigned char R;
		unsigned char G;
		unsigned char B;
		VoxelColor(unsigned char r, unsigned char g, unsigned char b) : R(r), G(g), B(b) {};
	};
	float **Pos = new float*[200];
	for (int i = 0; i < 200; i++)
		Pos[i] = new float[600];

	VoxelColor* ptr = (VoxelColor*)(void*)perlData;
	for (int i = 0; i < 200; i++)
	{
		for (int j = 0; j < 600; j++, ptr++)
		{
			Pos[i][j] = (float)(ptr->R / 5);
		}
	}
	return Pos;
}
