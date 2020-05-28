#pragma once

//Used Malloc so you need to free memory
//using DeleteMatrix(ptr, width); function 
float** CreateMatrix(const int& width, const int& height);

void DeleteMatrix(float** ptr, const int & width);

void SaveNoiseToPNG(float** noise, const int& width, const int& height, const char* filepath);

//noise - array[width][height] where will be stores Y coords,
//you can create it use CreateMatrix(width, height);
//a - horisontal + vertical scale
//b - vertical downscale
void GeneratePerlinNoise(float** noise, const int& width, const int& height, const float& a, const float& b,
	const int& octCount, const int& seedx = 0, const int& seedy = 0, const bool& periodic = false, const bool Normalize0_255 = false);

