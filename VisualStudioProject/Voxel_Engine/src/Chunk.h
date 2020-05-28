#pragma once

#include "opengl/VertexArray.h"
#include "opengl/VertexBuffer.h"
#include "glm/glm.hpp"

enum VoxelType : unsigned char //was enum class
{
	EMPTY,
	GRASS,
	WATER,
	SAND,
	DIRT,
	SNOW
};


class Chunk
{
private:
	//CPU DATA
	glm::ivec2 m_Pos; //position of a chunk (left up corner) 0,0 or 0,1 - to next chunk on z coord
	VoxelType*** m_Data;

	bool m_IsUpdated;

	unsigned int m_VoxelsCount;
	unsigned int m_FacesCount;
	unsigned int m_FacesTransparentCount;

	//for updating from other chunks
	//bool m_UpdateNearbyChunks; TODO

	//GPU DATA
	VertexArray m_VA;
	VertexBuffer m_VB;
	VertexArray m_VA_transparent;
	VertexBuffer m_VB_transparent;

	void fillDataFromNoise(float** Noise);

	//GPU
	void fillTransparentBuffer(unsigned int &x, unsigned int &y, unsigned int &z, unsigned char *buffer, unsigned int &ptr, 
							   Chunk *left = nullptr, Chunk *right = nullptr, 
							   Chunk *front = nullptr, Chunk *back = nullptr);
	void fillSolidBuffer(unsigned int &x, unsigned int &y, unsigned int &z, unsigned char *buffer, unsigned int &ptr,
						 Chunk *left = nullptr, Chunk *right = nullptr,
						 Chunk *front = nullptr, Chunk *back = nullptr);
	void Update(Chunk *left = nullptr, Chunk *right = nullptr,
				Chunk *front = nullptr, Chunk *back = nullptr);
	inline bool isTransparent(const VoxelType& voxel)
	{
		switch (voxel)
		{
		case GRASS: return false;
		case WATER: return true;
		default: return false;
		}
	}
public:
	Chunk(const glm::ivec2& pos, const glm::ivec2 & seed = {0, 0}); //TODO: implement "Draw buffer" which can draw a few chunk at one drawCall!
	~Chunk();

	inline bool IsGlobalCoordInside(const glm::vec3& pos)
	{
		glm::vec3 localPos = glm::vec3(pos.x - m_Pos.x * 16, pos.y, pos.z - m_Pos.y * 16);
		if (localPos.x >= 0.f && localPos.x < 16.f &&
			localPos.y >= 0.f && localPos.y < 255.f &&
			localPos.z >= 0.f && localPos.z < 16.f)
			return true;
		return false;
	}

	inline bool IsGlobalCoordInsideXZ(const glm::vec3& pos)
	{
		glm::vec3 localPos = glm::vec3(pos.x - m_Pos.x * 16, 0.f, pos.z - m_Pos.y * 16);
		if (localPos.x >= 0.f && localPos.x < 16.f &&
			localPos.z >= 0.f && localPos.z < 16.f)
			return true;
		return false;
	}

	//From chunk coords Not protected
	inline VoxelType&  GetVoxelType(const int& x, const int& y, const int& z) { return m_Data[x][y][z]; }

	inline VoxelType&  GetVoxelTypeGlobal(const int& x, const int& y, const int& z) { return m_Data[x - m_Pos.x * 16][y][z -  m_Pos.y * 16]; }

	//Not protected 
	void SetDataChunkCoords(const int& x, const int& y, const int& z, const VoxelType& type);

	//Not protected 
	void SetDataGlobalCoords(const int& x, const int& y, const int& z, const VoxelType& type,
							 Chunk *left = nullptr, Chunk *right = nullptr,
							 Chunk *front = nullptr, Chunk *back = nullptr);
	void Draw(Chunk *left = nullptr, Chunk *right = nullptr,
			  Chunk *front = nullptr, Chunk *back = nullptr);
	void DrawTransparent(Chunk *left = nullptr, Chunk *right = nullptr,
						 Chunk *front = nullptr, Chunk *back = nullptr);
	//
	void Refill(const glm::ivec2& pos, const glm::ivec2 & seed = {0, 0});

	//Inline
	glm::ivec2 GetPos() const { return m_Pos; }
	void SetPos(const glm::ivec2& pos) { m_Pos = pos; }
	void inline ForceUpdate(Chunk *left = nullptr, Chunk *right = nullptr, Chunk *front = nullptr, Chunk *back = nullptr) { Update(left, right, front, back); }

	friend class World;
	friend class ChunkChanges;
};

