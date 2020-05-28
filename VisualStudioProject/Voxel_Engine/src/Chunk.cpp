#include "Chunk.h"

#include "opengl/VertexBufferLayout.h"
#include "PerlinNoise.h"

#include "ChunkSize.h"

//Generator param-s
//const float ParamA = 0.2f;  //a - horisontal + vertical scale
//const float ParamB = 2.0f;  //b - vertical downscale
//const int octCount = 4;

//nice
//const float ParamA = 0.1f;  //a - horisontal + vertical scale
//const float ParamB = 2.0f;  //b - vertical downscale
//const int octCount = 4;

const int waterLevel = 120;
const int sandLevelstart = waterLevel - 10;
const int grassLevelstart = waterLevel + 4;
const int snowLevelstart = 200;

//ok
//const float ParamA = 0.01f;  //a - horisontal + vertical scale
//const float ParamB = 1.2f;  //b - vertical downscale
//const int octCount = 6; //6 is Ok

//const float ParamA = 16.f;  //a - horisontal + vertical scale
//const float ParamB = 3.0f;  //b - vertical downscale
//const int octCount = 1;

//NEW Methos

//const float ParamA = 0.05f;  //a - horisontal + vertical scale
//const float ParamB = 3.f;  //b - vertical downscale
//const int octCount = 6; //6 is Ok

const float ParamA = 0.04f;  //a - start freq
const float ParamB = 2.5f;  //b - downstep per oct
const int octCount = 6; //6 is Ok


const unsigned int VertexSizeCount = 6 * 5;
const unsigned int VertexSizeBytes = 6 * 5 * sizeof(unsigned char);

enum Side : unsigned char
{
	FRONT,
	LEFT,
	TOP,
	BOTTOM,
	RIGHT,
	BACK
};

//0 - front, 1 - left, 2 - top, 3 - bottom, 4 - right, 5 - back 
static void SetFace(unsigned char* arr, unsigned int& ptr, const unsigned char& x, const unsigned char& y, const unsigned char& z, const unsigned char& side, const unsigned char& texture);

static void SetFace(unsigned char* arr, unsigned int& ptr, const unsigned char& x, const unsigned char& y, const unsigned char& z,
					const unsigned char& sizex, const unsigned char& sizey, const unsigned char& sizez, const unsigned char& side, const unsigned char& texture);

#include <iostream>

void Chunk::fillDataFromNoise(float** Noise)
{
	m_IsUpdated = true;

	//OLD but 100% safety and works
	for (unsigned int x = 0; x < MaxSize; x++)
		for (unsigned int y = 0; y < MaxHeight; y++)
			for (unsigned int z = 0; z < MaxSize; z++)
				m_Data[x][y][z] = VoxelType::EMPTY;
	for (unsigned int i = 0; i < MaxSize; i++)
	{
		for (unsigned int j = 0; j < MaxSize; j++)
		{
			int height = int(Noise[i][j]);
			int heightTexture = height;

			if (heightTexture < sandLevelstart)
				m_Data[j][height][i] = VoxelType::DIRT;
			else if (heightTexture < grassLevelstart)
				m_Data[j][height][i] = VoxelType::SAND;
			else if (heightTexture < snowLevelstart)
				m_Data[j][height][i] = VoxelType::GRASS;
			else
				m_Data[j][height][i] = VoxelType::SNOW;

			for (int y = height + 1; y < waterLevel; y++)
				m_Data[j][y][i] = VoxelType::WATER;
			
			for (int y = height - 1; y >= 0; y--)
				m_Data[j][y][i] = VoxelType::DIRT;
		}
	}

	//Maby new
	//for (unsigned int i = 0; i < MaxSize; i++)
	//{
	//	for (unsigned int j = 0; j < MaxSize; j++)
	//	{
	//		m_Data[j][int(Noise[i][j])][i] = VoxelType::GRASS;
	//
	//		for (int y = int(Noise[i][j]) + 1; y < MaxHeight; y++)
	//		{
	//			if (y < waterLevel)
	//				m_Data[j][y][i] = VoxelType::WATER;
	//			else
	//				m_Data[j][y][i] = VoxelType::EMPTY;
	//		}
	//
	//		for (int y = int(Noise[i][j]) - 1; y >= 0; y--)
	//			m_Data[j][y][i] = VoxelType::GRASS;
	//	}
	//}
}



void Chunk::fillTransparentBuffer(unsigned int &x, unsigned int &y, unsigned int &z, unsigned char * buffer, unsigned int & ptr,
								  Chunk *left /*= nullptr*/, Chunk *right /*= nullptr*/,
								  Chunk *front /*= nullptr*/, Chunk *back /*= nullptr*/)
{
	//TEMP
	bool enableOutsideChunk = false;
	bool useNearbyChunks = true;

	/****************
	**    LEFT     **
	****************/
	if (x != 0)
	{
		if (m_Data[x - 1][y][z] == VoxelType::EMPTY)
		{
			SetFace(buffer, ptr, x, y, z, Side::LEFT, m_Data[x][y][z] - 1);
			++m_FacesTransparentCount;
		}
		else if (m_Data[x - 1][y][z] != m_Data[x][y][z] && isTransparent(m_Data[x - 1][y][z]))
		{
			SetFace(buffer, ptr, x, y, z, Side::LEFT, m_Data[x][y][z] - 1);
			++m_FacesTransparentCount;
		}
	}
	else if (enableOutsideChunk) // Left is edge of a chunk
	{
		SetFace(buffer, ptr, x, y, z, Side::LEFT, m_Data[x][y][z] - 1);
		++m_FacesTransparentCount;
	}
	else if (useNearbyChunks) //using left chunk for fill holes
	{
		if (left != nullptr)
		{
			if (left->m_Data[MaxSize - 1][y][z] == VoxelType::EMPTY)
			{
				SetFace(buffer, ptr, x, y, z, Side::LEFT, m_Data[x][y][z] - 1);
				++m_FacesTransparentCount;
			}
			else if (left->m_Data[MaxSize - 1][y][z] != m_Data[x][y][z] && isTransparent(left->m_Data[MaxSize - 1][y][z]))
			{
				SetFace(buffer, ptr, x, y, z, Side::LEFT, m_Data[x][y][z] - 1);
				++m_FacesTransparentCount;
			}
		}
	}

	/****************
	**    RIGHT    **
	****************/
	if (x != MaxSize - 1)
	{
		if (m_Data[x + 1][y][z] == VoxelType::EMPTY)
		{
			SetFace(buffer, ptr, x, y, z, Side::RIGHT, m_Data[x][y][z] - 1);
			++m_FacesTransparentCount;
		}
		else if (m_Data[x + 1][y][z] != m_Data[x][y][z] && isTransparent(m_Data[x + 1][y][z]))
		{
			SetFace(buffer, ptr, x, y, z, Side::RIGHT, m_Data[x][y][z] - 1);
			++m_FacesTransparentCount;
		}
	}
	else if (enableOutsideChunk) // Right is edge of a chunk
	{
		SetFace(buffer, ptr, x, y, z, Side::RIGHT, m_Data[x][y][z] - 1);
		++m_FacesTransparentCount;
	}
	else if (useNearbyChunks) //using left chunk for fill holes
	{
		if (right != nullptr)
		{
			if (right->m_Data[0][y][z] == VoxelType::EMPTY)
			{
				SetFace(buffer, ptr, x, y, z, Side::RIGHT, m_Data[x][y][z] - 1);
				++m_FacesTransparentCount;
			}
			else if (right->m_Data[0][y][z] != m_Data[x][y][z] && isTransparent(right->m_Data[0][y][z]))
			{
				SetFace(buffer, ptr, x, y, z, Side::RIGHT, m_Data[x][y][z] - 1);
				++m_FacesTransparentCount;
			}
		}
	}

	/****************
	**    BOTTOM   **
	****************/
	if (y != 0)
	{
		if (m_Data[x][y - 1][z] == VoxelType::EMPTY)
		{
			SetFace(buffer, ptr, x, y, z, Side::BOTTOM, m_Data[x][y][z] - 1);
			++m_FacesTransparentCount;
		}
		else if (m_Data[x][y - 1][z] != m_Data[x][y][z] && isTransparent(m_Data[x][y - 1][z]))
		{
			SetFace(buffer, ptr, x, y, z, Side::BOTTOM, m_Data[x][y][z] - 1);
			++m_FacesTransparentCount;
		}
	}
	else if (enableOutsideChunk) // Bottom is edge of a chunk
	{
		SetFace(buffer, ptr, x, y, z, Side::BOTTOM, m_Data[x][y][z] - 1);
		++m_FacesTransparentCount;
	}

	/****************
	**    TOP      **
	****************/
	if (y != MaxHeight - 1)
	{
		if (m_Data[x][y + 1][z] == VoxelType::EMPTY)
		{
			SetFace(buffer, ptr, x, y, z, Side::TOP, m_Data[x][y][z] - 1);
			++m_FacesTransparentCount;
		}
		else if (m_Data[x][y + 1][z] != m_Data[x][y][z] && isTransparent(m_Data[x][y + 1][z]))
		{
			SetFace(buffer, ptr, x, y, z, Side::TOP, m_Data[x][y][z] - 1);
			++m_FacesTransparentCount;
		}
		//ONLY FOR WATER TODO: Rewrite this stuff
		else if (m_Data[x][y][z] == VoxelType::WATER && !isTransparent(m_Data[x][y + 1][z]))
		{
			SetFace(buffer, ptr, x, y, z, Side::TOP, m_Data[x][y][z] - 1);
			++m_FacesTransparentCount;
		}
	}
	//WAS else if (enableOutsideChunk) // Top is edge of a chunk
	else // Top is edge of a chunk
	{
		SetFace(buffer, ptr, x, y, z, Side::TOP, m_Data[x][y][z] - 1);
		++m_FacesTransparentCount;
	}

	/****************
	**    BACK     **
	****************/
	if (z != 0)
	{
		if (m_Data[x][y][z - 1] == VoxelType::EMPTY)
		{
			SetFace(buffer, ptr, x, y, z, Side::BACK, m_Data[x][y][z] - 1);
			++m_FacesTransparentCount;
		}
		else if (m_Data[x][y][z - 1] != m_Data[x][y][z] && isTransparent(m_Data[x][y][z - 1]))
		{
			SetFace(buffer, ptr, x, y, z, Side::BACK, m_Data[x][y][z] - 1);
			++m_FacesTransparentCount;
		}
	}
	else if (enableOutsideChunk) //Back is edge of a chunk
	{
		SetFace(buffer, ptr, x, y, z, Side::BACK, m_Data[x][y][z] - 1);
		++m_FacesTransparentCount;
	}
	else if (useNearbyChunks) //using left chunk for fill holes
	{
		if (back != nullptr)
		{
			if (back->m_Data[x][y][MaxSize - 1] == VoxelType::EMPTY)
			{
				SetFace(buffer, ptr, x, y, z, Side::BACK, m_Data[x][y][z] - 1);
				++m_FacesTransparentCount;
			}
			else if (back->m_Data[x][y][MaxSize - 1] != m_Data[x][y][z] && isTransparent(back->m_Data[x][y][MaxSize - 1]))
			{
				SetFace(buffer, ptr, x, y, z, Side::BACK, m_Data[x][y][z] - 1);
				++m_FacesTransparentCount;
			}
		}
	}

	/****************
	**    FRONT    **
	****************/
	if (z != MaxSize - 1)
	{
		if (m_Data[x][y][z + 1] == VoxelType::EMPTY)
		{
			SetFace(buffer, ptr, x, y, z, Side::FRONT, m_Data[x][y][z] - 1);
			++m_FacesTransparentCount;
		}
		else if (m_Data[x][y][z + 1] != m_Data[x][y][z] && isTransparent(m_Data[x][y][z + 1]))
		{
			SetFace(buffer, ptr, x, y, z, Side::FRONT, m_Data[x][y][z] - 1);
			++m_FacesTransparentCount;
		}

	}
	else if (enableOutsideChunk) //Front is edge of a chunk
	{
		SetFace(buffer, ptr, x, y, z, Side::FRONT, m_Data[x][y][z] - 1);
		++m_FacesTransparentCount;
	}
	else if (useNearbyChunks) //using left chunk for fill holes
	{
		if (front != nullptr)
		{
			if (front->m_Data[x][y][0] == VoxelType::EMPTY)
			{
				SetFace(buffer, ptr, x, y, z, Side::FRONT, m_Data[x][y][z] - 1);
				++m_FacesTransparentCount;
			}
			else if (front->m_Data[x][y][0] != m_Data[x][y][z] && isTransparent(front->m_Data[x][y][0]))
			{
				SetFace(buffer, ptr, x, y, z, Side::FRONT, m_Data[x][y][z] - 1);
				++m_FacesTransparentCount;
			}
		}
	}
}

void Chunk::fillSolidBuffer(unsigned int &x, unsigned int &y, unsigned int &z, unsigned char * buffer, unsigned int & ptr,
							Chunk *left /*= nullptr*/, Chunk *right /*= nullptr*/,
							Chunk *front /*= nullptr*/, Chunk *back /*= nullptr*/)
{
	//TEMP
	bool enableOutsideChunk = false;
	bool enableTranspanent = true;
	bool useNearbyChunks = true;

	/****************
	**    LEFT     **
	****************/
	if (x != 0)
	{
		if (m_Data[x - 1][y][z] == VoxelType::EMPTY)
		{
			SetFace(buffer, ptr, x, y, z, Side::LEFT, m_Data[x][y][z] - 1);
			++m_FacesCount;
		}
		else if (enableTranspanent)
		{
			if (isTransparent(m_Data[x - 1][y][z]))
			{
				SetFace(buffer, ptr, x, y, z, Side::LEFT, m_Data[x][y][z] - 1);
				++m_FacesCount;
			}
		}
	}
	else if (enableOutsideChunk) // Left is edge of a chunk
	{
		SetFace(buffer, ptr, x, y, z, Side::LEFT, m_Data[x][y][z] - 1);
		++m_FacesCount;
	}
	else if (useNearbyChunks) //using left chunk for fill holes
	{
		if (left != nullptr)
		{
			if (left->m_Data[MaxSize - 1][y][z] == VoxelType::EMPTY)
			{
				SetFace(buffer, ptr, x, y, z, Side::LEFT, m_Data[x][y][z] - 1);
				++m_FacesCount;
			}
			else if (enableTranspanent)
			{
				if (isTransparent(left->m_Data[MaxSize - 1][y][z]))
				{
					SetFace(buffer, ptr, x, y, z, Side::LEFT, m_Data[x][y][z] - 1);
					++m_FacesCount;
				}
			}
			//TODO: fix holes betwen chunks
		}
	}

	/****************
	**    RIGHT    **
	****************/
	if (x != MaxSize - 1)
	{
		if (m_Data[x + 1][y][z] == VoxelType::EMPTY)
		{
			SetFace(buffer, ptr, x, y, z, Side::RIGHT, m_Data[x][y][z] - 1);
			++m_FacesCount;
		}
		else if (enableTranspanent)
		{
			if (isTransparent(m_Data[x + 1][y][z]))
			{
				SetFace(buffer, ptr, x, y, z, Side::RIGHT, m_Data[x][y][z] - 1);
				++m_FacesCount;
			}
		}
	}
	else if (enableOutsideChunk) // Right is edge of a chunk
	{
		SetFace(buffer, ptr, x, y, z, Side::RIGHT, m_Data[x][y][z] - 1);
		++m_FacesCount;
	}
	else if (useNearbyChunks) //using left chunk for fill holes
	{
		if (right != nullptr)
		{
			if (right->m_Data[0][y][z] == VoxelType::EMPTY)
			{
				SetFace(buffer, ptr, x, y, z, Side::RIGHT, m_Data[x][y][z] - 1);
				++m_FacesCount;
			}
			else if (enableTranspanent)
			{
				if (isTransparent(right->m_Data[0][y][z]))
				{
					SetFace(buffer, ptr, x, y, z, Side::RIGHT, m_Data[x][y][z] - 1);
					++m_FacesCount;
				}
			}
		}
	}
	
	/****************
	**    BOTTOM   **
	****************/
	if (y != 0)
	{
		if (m_Data[x][y - 1][z] == VoxelType::EMPTY)
		{
			SetFace(buffer, ptr, x, y, z, Side::BOTTOM, m_Data[x][y][z] - 1);
			++m_FacesCount;
		}
		else if (enableTranspanent)
		{
			if (isTransparent(m_Data[x][y - 1][z]))
			{
				SetFace(buffer, ptr, x, y, z, Side::BOTTOM, m_Data[x][y][z] - 1);
				++m_FacesCount;
			}
		}
	}
	else if (enableOutsideChunk) // Bottom is edge of a chunk
	{
		SetFace(buffer, ptr, x, y, z, Side::BOTTOM, m_Data[x][y][z] - 1);
		++m_FacesCount;
	}

	/****************
	**    TOP      **
	****************/
	if (y != MaxHeight - 1)
	{
		if (m_Data[x][y + 1][z] == VoxelType::EMPTY)
		{
			SetFace(buffer, ptr, x, y, z, Side::TOP, m_Data[x][y][z] - 1);
			++m_FacesCount;
		}
		else if (enableTranspanent)
		{
			if (isTransparent(m_Data[x][y + 1][z]))
			{
				SetFace(buffer, ptr, x, y, z, Side::TOP, m_Data[x][y][z] - 1);
				++m_FacesCount;
			}
		}
	}
	//WAS else if (enableOutsideChunk) // Top is edge of a chunk
	else // Top is edge of a chunk
	{
		SetFace(buffer, ptr, x, y, z, Side::TOP, m_Data[x][y][z] - 1);
		++m_FacesCount;
	}

	/****************
	**    BACK     **
	****************/
	if (z != 0)
	{
		if (m_Data[x][y][z - 1] == VoxelType::EMPTY)
		{
			SetFace(buffer, ptr, x, y, z, Side::BACK, m_Data[x][y][z] - 1);
			++m_FacesCount;
		}
		else if (enableTranspanent)
		{
			if (isTransparent(m_Data[x][y][z - 1]))
			{
				SetFace(buffer, ptr, x, y, z, Side::BACK, m_Data[x][y][z] - 1);
				++m_FacesCount;
			}
		}
	}
	else if (enableOutsideChunk) //Back is edge of a chunk
	{
		SetFace(buffer, ptr, x, y, z, Side::BACK, m_Data[x][y][z] - 1);
		++m_FacesCount;
	}
	else if (useNearbyChunks) //using left chunk for fill holes
	{
		if (back != nullptr)
		{
			if (back->m_Data[x][y][MaxSize - 1] == VoxelType::EMPTY)
			{
				SetFace(buffer, ptr, x, y, z, Side::BACK, m_Data[x][y][z] - 1);
				++m_FacesCount;
			}
			else if (enableTranspanent)
			{
				if (isTransparent(back->m_Data[x][y][MaxSize - 1]))
				{
					SetFace(buffer, ptr, x, y, z, Side::BACK, m_Data[x][y][z] - 1);
					++m_FacesCount;
				}
			}
		}
	}

	/****************
	**    FRONT    **
	****************/
	if (z != MaxSize - 1)
	{
		if (m_Data[x][y][z + 1] == VoxelType::EMPTY)
		{
			SetFace(buffer, ptr, x, y, z, Side::FRONT, m_Data[x][y][z] - 1);
			++m_FacesCount;
		}
		else if (enableTranspanent)
		{
			if (isTransparent(m_Data[x][y][z + 1]))
			{
				SetFace(buffer, ptr, x, y, z, Side::FRONT, m_Data[x][y][z] - 1);
				++m_FacesCount;
			}
		}
	}
	else if (enableOutsideChunk) //Front is edge of a chunk
	{
		SetFace(buffer, ptr, x, y, z, Side::FRONT, m_Data[x][y][z] - 1);
		++m_FacesCount;
	}
	else if (useNearbyChunks) //using left chunk for fill holes
	{
		if (front != nullptr)
		{
			if (front->m_Data[x][y][0] == VoxelType::EMPTY)
			{
				SetFace(buffer, ptr, x, y, z, Side::FRONT, m_Data[x][y][z] - 1);
				++m_FacesCount;
			}
			else if (enableTranspanent)
			{
				if (isTransparent(front->m_Data[x][y][0]))
				{
					SetFace(buffer, ptr, x, y, z, Side::FRONT, m_Data[x][y][z] - 1);
					++m_FacesCount;
				}
			}
		}
	}
}

void Chunk::Update(Chunk *left /*= nullptr*/, Chunk *right /*= nullptr*/,
				   Chunk *front /*= nullptr*/, Chunk *back /*= nullptr*/)
{
	//TEMP
	//bool enableOutsideChunk = false;
	//bool enableTranspanent = true;

	m_IsUpdated = false;

	unsigned char *buffer = new unsigned char[16 * 256 * 16 * VertexSizeBytes * 4];
	unsigned char *bufferTransparent = new unsigned char[16 * 256 * 16 * VertexSizeBytes * 4];
	unsigned int ptr = 0;
	unsigned int ptrTransparent = 0;

	m_FacesCount = 0;
	m_FacesTransparentCount = 0;
	m_VoxelsCount = 0;
	for (unsigned int x = 0; x < MaxSize; x++) //TODO: GREEDY MESHING
	{
		for (unsigned int y = 0; y < MaxHeight; y++)
		{
			for (unsigned int z = 0; z < MaxSize; z++)
			{
				if (m_Data[x][y][z] == VoxelType::EMPTY)
					continue;
				++m_VoxelsCount;
				if (isTransparent(m_Data[x][y][z])) // Set GPU memory only for transparent voxels
				{
					fillTransparentBuffer(x, y, z, bufferTransparent, ptrTransparent, left, right, front, back);
				}
				else // Set GPU memory only for NONtransparent voxels
				{
					fillSolidBuffer(x, y, z, buffer, ptr, left, right, front, back);
				}
			}
		}
	}
	m_VB.Create(buffer, m_FacesCount * VertexSizeBytes, VertexBuffer::DrawType::STATIC);
	m_VB_transparent.Create(bufferTransparent, m_FacesTransparentCount * VertexSizeBytes, VertexBuffer::DrawType::STATIC);
	delete[] buffer;
	delete[] bufferTransparent;
}

/*
struct Face
{
	//if look from top and left top corner is begin then:
  
	unsigned char posY;    //from up to Down
	unsigned char sizeX;   //size horizontal
	unsigned char sizeY;   //size vertical
	unsigned char texture;

	Face() : posY(0), sizeX(0), sizeY(0), texture(0) {}
	Face(unsigned char pY, unsigned char sX, unsigned char sY, unsigned char text) : posY(pY), sizeX(sX), sizeY(sY), texture(text) {}
	void Clear() { posY = unsigned char(0); sizeX = unsigned char(0); sizeY = unsigned char(0); texture = unsigned char(0);}
	void Add()
	{
		++sizeY;
	}
	bool isEmpty()
	{
		return sizeX == unsigned char(0);
	}
	void Set(unsigned char size_X, unsigned char text)
	{
		posY = 1;
		sizeX = size_X;
		texture = text;
	}
};
struct FaceLine
{
	//if look from top and left top corner is begin then:

	unsigned char arrPtr; //Pointer in array from left to Right
	unsigned char size;	   //size horizontal
	unsigned char texture;

	FaceLine() : size(0), texture(0), arrPtr(0) {}
	FaceLine(unsigned char pX, unsigned char s, unsigned char text) : arrPtr(pX), size(s), texture(text) {}
	void Clear() { size = unsigned char(0); texture = unsigned char(0); arrPtr = unsigned char(0); }
	void Add()
	{
		++size;
	}
	bool isEmpty()
	{
		return size == unsigned char(0);
	}
	void Set(unsigned char arrptr, unsigned char text)
	{
		arrPtr = arrptr;
		size = 1;
		texture = text;
	}
};

bool operator==(const FaceLine& fl, const Face& f)
{
	return (f.sizeX == fl.size) && (f.texture == fl.texture);
}
bool operator<=(const FaceLine& fl, const Face& f)
{
	return (fl.size <= f.sizeX) && (f.texture == fl.texture);
}
bool operator<(const FaceLine& fl, const Face& f)
{
	return (fl.size < f.sizeX) && (f.texture == fl.texture);
}
*/

Chunk::Chunk(const glm::ivec2 & pos, const glm::ivec2 & seed)
	:m_Pos(pos), m_IsUpdated(true), m_VoxelsCount(0), m_FacesCount(0), m_FacesTransparentCount(0)
{
	//Create DataBuffer
	m_Data = new VoxelType**[MaxSize];
	for (unsigned int k = 0; k < MaxSize; k++)
	{
		m_Data[k] = new VoxelType*[MaxHeight];
		for (unsigned int i = 0; i < MaxHeight; i++)
			m_Data[k][i] = new VoxelType[MaxSize]{VoxelType::EMPTY};
	}


	VertexBufferLayout layout;
	layout.Push<unsigned char>(3); // pos
	layout.Push<unsigned char>(1); // texture coords
	layout.Push<unsigned char>(1); // normale vectors
	m_VA.AddBuffer(m_VB, layout);
	m_VA_transparent.AddBuffer(m_VB_transparent, layout);

	float** Noise = CreateMatrix(int(MaxSize), int(MaxSize));
	
	GeneratePerlinNoise(Noise, int(MaxSize), int(MaxSize), ParamA, ParamB, octCount, seed.y + pos.y * MaxSize, seed.x + pos.x * MaxSize, false, true);
	fillDataFromNoise(Noise);
	DeleteMatrix(Noise, int(MaxSize));
}

Chunk::~Chunk()
{
	//Delete DataBuffer
	for (unsigned int k = 0; k < MaxSize; k++)
	{
		for (unsigned int i = 0; i < MaxHeight; i++)
			delete[] m_Data[k][i];
		delete[] m_Data[k];
	}
	delete[] m_Data;
}

void Chunk::SetDataChunkCoords(const int & x, const int & y, const int & z, const VoxelType & type)
{
	m_Data[x][y][z] = type;
	m_IsUpdated = true;

	//if (x == MaxSize - 1) TODO update nearby chunks
}

void Chunk::SetDataGlobalCoords(const int & x, const int & y, const int & z, const VoxelType & type,
								Chunk *left /*= nullptr*/, Chunk *right /*= nullptr*/,
								Chunk *front /*= nullptr*/, Chunk *back /*= nullptr*/)
{
	//Chunk coords
	int X = x - m_Pos.x * MaxSize; 
	int Z = z - m_Pos.y * MaxSize;

	m_Data[X][y][Z] = type;

	if (X == 0 && left != nullptr)
		left->m_IsUpdated = true;
	if (X == MaxSize - 1 && right != nullptr)
		right->m_IsUpdated = true;
	if (Z == 0 && back != nullptr)
		back->m_IsUpdated = true;
	if (Z == MaxSize - 1 && front != nullptr)
		front->m_IsUpdated = true;

	m_IsUpdated = true;
}

void Chunk::Draw(Chunk *left /*= nullptr*/, Chunk *right /*= nullptr*/,
				 Chunk *front /*= nullptr*/, Chunk *back /*= nullptr*/)
{
	if (m_IsUpdated)
		Update(left, right, front, back);

	if (!m_FacesCount)
		return;

	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);

	m_VA.Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, m_FacesCount * 6));
}
void Chunk::DrawTransparent(Chunk *left /*= nullptr*/, Chunk *right /*= nullptr*/,
							Chunk *front /*= nullptr*/, Chunk *back /*= nullptr*/)
{
	//if (m_IsUpdated)
	//	Update(left, right, front, back);
	//

	if (!m_FacesTransparentCount)
		return;

	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);

	m_VA_transparent.Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, m_FacesTransparentCount * 6));
}

void Chunk::Refill(const glm::ivec2& pos, const glm::ivec2 & seed /*= { 0, 0 }*/)
{
	m_Pos = pos;
	float** Noise = CreateMatrix(int(MaxSize), int(MaxSize));
	GeneratePerlinNoise(Noise, int(MaxSize), int(MaxSize), ParamA, ParamB, octCount, seed.y + pos.y * MaxSize, seed.x + pos.x * MaxSize, false, true);
	fillDataFromNoise(Noise);
	DeleteMatrix(Noise, int(MaxSize));
}







// Static
static void SetFace(unsigned char* arr, unsigned int& ptr, const unsigned char& x, const unsigned char& y, const unsigned char& z, const unsigned char& side, const unsigned char& texture)
{
	unsigned char temp[VertexSizeCount];

	if (side == Side::FRONT)
	{
		unsigned char tmp[] = {
			unsigned char(0 + x), unsigned char(1 + y), unsigned char(1 + z),  unsigned char(texture * 4),        side,  //0
			unsigned char(0 + x), unsigned char(0 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 2),    side,  //2
			unsigned char(1 + x), unsigned char(1 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 1),    side,  //1   FRONT
			unsigned char(1 + x), unsigned char(1 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 1),    side,  //1
			unsigned char(0 + x), unsigned char(0 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 2),    side,  //2
			unsigned char(1 + x), unsigned char(0 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 3),    side   //3
		};
		memcpy(temp, tmp, VertexSizeBytes);
	}
	else if (side == Side::LEFT)
	{
		unsigned char tmp[] = {
			unsigned char(0 + x), unsigned char(1 + y), unsigned char(0 + z),  unsigned char(texture * 4),        side,  //4
			unsigned char(0 + x), unsigned char(0 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 2),    side,  //6
			unsigned char(0 + x), unsigned char(1 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 1),    side,  //5   LEFT
			unsigned char(0 + x), unsigned char(1 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 1),    side,  //5
			unsigned char(0 + x), unsigned char(0 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 2),    side,  //6
			unsigned char(0 + x), unsigned char(0 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 3),    side   //7
		};
		memcpy(temp, tmp, VertexSizeBytes);
	}
	else if (side == Side::TOP)
	{
		unsigned char tmp[] = {
			unsigned char(0 + x), unsigned char(1 + y), unsigned char(0 + z),  unsigned char(texture * 4),        side,  //8
			unsigned char(0 + x), unsigned char(1 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 2),    side,  //10
			unsigned char(1 + x), unsigned char(1 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 1),    side,  //9   TOP
			unsigned char(1 + x), unsigned char(1 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 1),    side,  //9
			unsigned char(0 + x), unsigned char(1 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 2),    side,  //10
			unsigned char(1 + x), unsigned char(1 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 3),    side   //11
		};
		memcpy(temp, tmp, VertexSizeBytes);
	}
	else if (side == Side::BOTTOM)
	{
		unsigned char tmp[] = {
			unsigned char(0 + x), unsigned char(0 + y), unsigned char(1 + z),  unsigned char(texture * 4),        side,  //12
			unsigned char(0 + x), unsigned char(0 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 2),    side,  //14
			unsigned char(1 + x), unsigned char(0 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 1),    side,  //13  BOTTOM
			unsigned char(1 + x), unsigned char(0 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 1),    side,  //13
			unsigned char(0 + x), unsigned char(0 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 2),    side,  //14
			unsigned char(1 + x), unsigned char(0 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 3),    side   //15
		};
		memcpy(temp, tmp, VertexSizeBytes);
	}
	else if (side == Side::RIGHT)
	{
		unsigned char tmp[] = {
			unsigned char(1 + x), unsigned char(1 + y), unsigned char(1 + z),  unsigned char(texture * 4),        side,  //16
			unsigned char(1 + x), unsigned char(0 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 2),    side,  //18
			unsigned char(1 + x), unsigned char(1 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 1),    side,  //17  RIGHT
			unsigned char(1 + x), unsigned char(1 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 1),    side,  //17
			unsigned char(1 + x), unsigned char(0 + y), unsigned char(1 + z),  unsigned char(texture * 4 + 2),    side,  //18
			unsigned char(1 + x), unsigned char(0 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 3),    side   //19
		};
		memcpy(temp, tmp, VertexSizeBytes);
	}
	else if (side == Side::BACK)
	{
		unsigned char tmp[] = {
			unsigned char(1 + x), unsigned char(1 + y), unsigned char(0 + z),  unsigned char(texture * 4),        side,  //20
			unsigned char(1 + x), unsigned char(0 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 2),    side,  //22
			unsigned char(0 + x), unsigned char(1 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 1),    side,  //21  BACK
			unsigned char(0 + x), unsigned char(1 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 1),    side,  //21
			unsigned char(1 + x), unsigned char(0 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 2),    side,  //22
			unsigned char(0 + x), unsigned char(0 + y), unsigned char(0 + z),  unsigned char(texture * 4 + 3),    side   //23
		};
		memcpy(temp, tmp, VertexSizeBytes);
	}

	for (unsigned int i = 0; i < VertexSizeCount; ++i, ++ptr)
		arr[ptr] = temp[i];
}




static void SetFace(unsigned char* arr, unsigned int& ptr, const unsigned char& x, const unsigned char& y, const unsigned char& z,
	const unsigned char& sizex, const unsigned char& sizey, const unsigned char& sizez, const unsigned char& side, const unsigned char& texture)
{
	unsigned char temp[VertexSizeCount];

	if (side == Side::FRONT)
	{
		unsigned char tmp[] = {
			unsigned char(0 + x),     unsigned char(y + sizey), unsigned char(z + sizez),  unsigned char(texture * 4),        side,  //0
			unsigned char(0 + x),     unsigned char(0 + y),     unsigned char(z + sizez),  unsigned char(texture * 4 + 2),    side,  //2
			unsigned char(x + sizex), unsigned char(y + sizey), unsigned char(z + sizez),  unsigned char(texture * 4 + 1),    side,  //1   FRONT
			unsigned char(x + sizex), unsigned char(y + sizey), unsigned char(z + sizez),  unsigned char(texture * 4 + 1),    side,  //1
			unsigned char(0 + x),     unsigned char(0 + y),     unsigned char(z + sizez),  unsigned char(texture * 4 + 2),    side,  //2
			unsigned char(x + sizex), unsigned char(0 + y),     unsigned char(z + sizez),  unsigned char(texture * 4 + 3),    side   //3
		};
		memcpy(temp, tmp, VertexSizeBytes);
	}
	else if (side == Side::LEFT)
	{
		unsigned char tmp[] = {
			unsigned char(0 + x),     unsigned char(y + sizey), unsigned char(0 + z),      unsigned char(texture * 4),        side,  //4
			unsigned char(0 + x),     unsigned char(0 + y),     unsigned char(0 + z),      unsigned char(texture * 4 + 2),    side,  //6
			unsigned char(0 + x),     unsigned char(y + sizey), unsigned char(z + sizez),  unsigned char(texture * 4 + 1),    side,  //5   LEFT
			unsigned char(0 + x),     unsigned char(y + sizey), unsigned char(z + sizez),  unsigned char(texture * 4 + 1),    side,  //5
			unsigned char(0 + x),     unsigned char(0 + y),     unsigned char(0 + z),      unsigned char(texture * 4 + 2),    side,  //6
			unsigned char(0 + x),     unsigned char(0 + y),     unsigned char(z + sizez),  unsigned char(texture * 4 + 3),    side   //7
		};
		memcpy(temp, tmp, VertexSizeBytes);
	}
	else if (side == Side::TOP)
	{
		unsigned char tmp[] = {
			unsigned char(0 + x),     unsigned char(y + sizey), unsigned char(0 + z),      unsigned char(texture * 4),        side,  //8
			unsigned char(0 + x),     unsigned char(y + sizey), unsigned char(z + sizez),  unsigned char(texture * 4 + 2),    side,  //10
			unsigned char(x + sizex), unsigned char(y + sizey), unsigned char(0 + z),      unsigned char(texture * 4 + 1),    side,  //9   TOP
			unsigned char(x + sizex), unsigned char(y + sizey), unsigned char(0 + z),      unsigned char(texture * 4 + 1),    side,  //9
			unsigned char(0 + x),     unsigned char(y + sizey), unsigned char(z + sizez),  unsigned char(texture * 4 + 2),    side,  //10
			unsigned char(x + sizex), unsigned char(y + sizey), unsigned char(z + sizez),  unsigned char(texture * 4 + 3),    side   //11
		};
		memcpy(temp, tmp, VertexSizeBytes);
	}
	else if (side == Side::BOTTOM)
	{
		unsigned char tmp[] = {
			unsigned char(0 + x),     unsigned char(0 + y),     unsigned char(z + sizez),  unsigned char(texture * 4),        side,  //12
			unsigned char(0 + x),     unsigned char(0 + y),     unsigned char(0 + z),      unsigned char(texture * 4 + 2),    side,  //14
			unsigned char(x + sizex), unsigned char(0 + y),     unsigned char(z + sizez),  unsigned char(texture * 4 + 1),    side,  //13  BOTTOM
			unsigned char(x + sizex), unsigned char(0 + y),     unsigned char(z + sizez),  unsigned char(texture * 4 + 1),    side,  //13
			unsigned char(0 + x),     unsigned char(0 + y),     unsigned char(0 + z),      unsigned char(texture * 4 + 2),    side,  //14
			unsigned char(x + sizex), unsigned char(0 + y),     unsigned char(0 + z),      unsigned char(texture * 4 + 3),    side   //15
		};
		memcpy(temp, tmp, VertexSizeBytes);
	}
	else if (side == Side::RIGHT)
	{
		unsigned char tmp[] = {
			unsigned char(x + sizex), unsigned char(y + sizey), unsigned char(z + sizez),  unsigned char(texture * 4),        side,  //16
			unsigned char(x + sizex), unsigned char(0 + y),     unsigned char(z + sizez),  unsigned char(texture * 4 + 2),    side,  //18
			unsigned char(x + sizex), unsigned char(y + sizey), unsigned char(0 + z),      unsigned char(texture * 4 + 1),    side,  //17  RIGHT
			unsigned char(x + sizex), unsigned char(y + sizey), unsigned char(0 + z),      unsigned char(texture * 4 + 1),    side,  //17
			unsigned char(x + sizex), unsigned char(0 + y),     unsigned char(z + sizez),  unsigned char(texture * 4 + 2),    side,  //18
			unsigned char(x + sizex), unsigned char(0 + y),     unsigned char(0 + z),      unsigned char(texture * 4 + 3),    side   //19
		};
		memcpy(temp, tmp, VertexSizeBytes);
	}
	else if (side == Side::BACK)
	{
		unsigned char tmp[] = {
			unsigned char(x + sizex), unsigned char(y + sizey), unsigned char(0 + z),      unsigned char(texture * 4),        side,  //20
			unsigned char(x + sizex), unsigned char(0 + y),     unsigned char(0 + z),      unsigned char(texture * 4 + 2),    side,  //22
			unsigned char(0 + x),     unsigned char(y + sizey), unsigned char(0 + z),      unsigned char(texture * 4 + 1),    side,  //21  BACK
			unsigned char(0 + x),     unsigned char(y + sizey), unsigned char(0 + z),      unsigned char(texture * 4 + 1),    side,  //21
			unsigned char(x + sizex), unsigned char(0 + y),     unsigned char(0 + z),      unsigned char(texture * 4 + 2),    side,  //22
			unsigned char(0 + x),     unsigned char(0 + y),     unsigned char(0 + z),      unsigned char(texture * 4 + 3),    side   //23
		};
		memcpy(temp, tmp, VertexSizeBytes);
	}

	for (unsigned int i = 0; i < VertexSizeCount; ++i, ++ptr)
		arr[ptr] = temp[i];
}
