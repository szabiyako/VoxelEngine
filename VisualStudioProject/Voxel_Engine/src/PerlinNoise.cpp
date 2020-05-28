#include "PerlinNoise.h"

#include "glm/glm.hpp"

//perlin noise
#include <glm/gtc/noise.hpp> 

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "ChunkSize.h"

//for debugging
#include <iostream> 

float** CreateMatrix(const int & width, const int & height)
{
	float** noise = new float*[width];
	for (int i = 0; i < width; i++)
	{
		noise[i] = new float[height];
		for (int j = 0; j < height; j++)
		{
			noise[i][j] = 0.f;
		}
	}
	return noise;
}

void DeleteMatrix(float** ptr, const int & width)
{
	for (int i = 0; i < width; i++)
		delete[] ptr[i];
	delete[] ptr;
}

void SaveNoiseToPNG(float ** noise, const int & width, const int & height, const char * filepath)
{
	float min = 999999.f;
	float max = -999999.f;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if (noise[i][j] > max)
				max = noise[i][j];
			if (noise[i][j] < min)
				min = noise[i][j];
		}
	}
	float tmp = max-min;
	struct RGBA
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
		RGBA(unsigned char r, unsigned char b, unsigned char g, unsigned char a) : r(r), g(g), b(b), a(a) {}
		RGBA(unsigned char val) : r(val), g(val), b(val), a(255) {}
		RGBA() : r(0), g(0), b(0), a(255) {}
	};
	int size = width * height;
	RGBA* image = new RGBA[size];
	for (int i = 0; i < size; i++)
	{
		image[i] = RGBA((unsigned char)(((noise[i / width][i % height] - min)/(tmp)) * 255));
	}

	stbi_write_png(filepath, width, height, 4, (const void*)image, 4 * width);

	delete[] image;
}

// Old method
//void GeneratePerlinNoise(float** noise, const int& width, const int& height, const float& a, const float& b,
//	const int& octCount, const int& seedx, const int& seedy, const bool& periodic/* = false*/, const bool Normalize0_255 /*= false*/)
//{
//	//Create buffer
//	float*** arr = new float**[octCount];
//	for (int k = 0; k < octCount; k++)
//	{
//		arr[k] = new float*[width];
//		for (int i = 0; i < width; i++)
//			arr[k][i] = new float[height];
//	}
//
//	//Create Noise (layer by layer)
//	float xF = 1.f / (width - 1);
//	float yF = 1.f / (height - 1);
//	for (int i = 0; i < width; i++)
//	{
//		for (int j = 0; j < height; j++)
//		{
//			float x = xF * (i + seedx);
//			float y = yF * (j + seedy);
//
//			float frec = a;
//			float scale = b;
//			float sum = 0.f;
//			for (int oct = 0; oct < octCount; oct++)
//			{
//				//sum = 0.f;
//				glm::vec2 tmp(x * frec, y * frec);
//				float val;
//				if (periodic)
//					val = glm::perlin(tmp, glm::vec2(frec)) / scale;
//				else
//					val = glm::perlin(tmp) / scale;
//				sum += val;
//				arr[oct][i][j] = ((sum + 1.f) / 2.f) * (float)(MaxHeight - 1);
//
//				frec *= 2;
//				scale *= b;
//			}
//		}
//	}
//
//	//Summing octavs
//	float min = 99.f;
//	float max = -99.f;
//	for (int i = 0; i < width; i++)
//	{
//		//noise[i] = new float[height];
//		for (int j = 0; j < height; j++)
//		{
//			noise[i][j] = 0.f; //clear buffer before filling
//			for (int k = 0; k < octCount; k++)
//				noise[i][j] += arr[k][i][j];
//			noise[i][j] = (float)(int)(noise[i][j]/octCount);
//			if (Normalize0_255)
//			{
//				//if (noise[i][j] > 255.f)
//				//	noise[i][j] = 255.f;
//				//else if (noise[i][j] < 0.f)
//				//	noise[i][j] = 0.f;
//				if (noise[i][j] > (float)(MaxHeight - 1))
//					noise[i][j] = (float)(MaxHeight - 1);
//				else if (noise[i][j] < 0.f)
//					noise[i][j] = 0.f;
//			}
//		}
//	}
//	//Delete buffer
//	for (int k = 0; k < octCount; k++)
//	{
//		for (int i = 0; i < width; i++)
//			delete[] arr[k][i];
//		delete[] arr[k];
//	}
//	delete[] arr;
//}

void GeneratePerlinNoise(float** noise, const int& width, const int& height, const float& a, const float& b,
	const int& octCount, const int& seedx, const int& seedy, const bool& periodic/* = false*/, const bool Normalize0_255 /*= false*/)
{
	//Create Noise (layer by layer)
	float xF = 1.f / (width - 1);
	float yF = 1.f / (height - 1);
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			float x = xF * (i + seedx);
			float y = yF * (j + seedy);

			float frec = a;
			float power = 1.f;
			float sum = 0.f;
			float scale = 1.f;
			//float scale = 0.5f *sqrt(4.f);
			float scalePower = 0.f;
			for (int oct = 0; oct < octCount; oct++)
			{
				scalePower += power;
				glm::vec2 tmp(x * frec, y * frec);
				float val;
				if (periodic)
					val = glm::perlin(tmp, glm::vec2(frec)) * power;
				else
					val = glm::perlin(tmp) * power;
				sum += val;

				frec *= 2;
				power /= b;
			}
			noise[i][j] = (float)((sum + scale * scalePower) / (2 * scale*scalePower));
			if (Normalize0_255)
				noise[i][j] = float(int((float)(MaxHeight - 1) * noise[i][j]));
		}
	}
}
