#pragma once

#include "glm/glm.hpp"
#include "Chunk.h"
#include "Camera.h"
#include "opengl/Shader.h"
#include "opengl/Texture.h"

#include "ChunkChanges.h"

class World
{
private:
	const unsigned int m_VisibleRange;
	Chunk*** m_ChunksPtr;
	Chunk*** m_BufferPtr;
	const unsigned int m_ChunksMatrixSize;

	//World changes
	ChunkChanges m_ChunkChanges;
	

	Shader m_Shader;

	Texture m_GrassTexture;
	Texture m_WaterTexture;
	Texture m_SandTexture;
	Texture m_DirtTexture;
	Texture m_SnowTexture;

	bool m_IsChunksPosUpdated;
	bool m_IsChunksNeedsReposition;
	bool **m_ChunksRefillNeeds;
	bool **m_ChunksRefillNeedsBuffer;

	glm::ivec2 m_Seed;
	glm::ivec2 m_PlayerChunkPos;
	glm::ivec2 m_MidPos;

	float m_WaterTime;

	glm::ivec2 GetChunkPos(const glm::vec3& pos);

	bool m_IsUpdatePosStored;
	int m_UpdatePosLeftUp;
	unsigned int m_UpdatePosDelta;
	void RefillChunks();
	void UpdateChunk(const int i, const int j);

	bool **m_IsInsideFrustum;
	bool isPointInsideFrustum(const glm::vec3 &point, float frustum[6][4]);
	bool isChunkInsideFrustum(const Chunk *chunk, float frustum[6][4]);
	bool FrustumCullingCheck(Camera& cam, const Chunk *chunk);

	inline Chunk* GetLeftChunk(int i, int j)
	{
		if (i != 0)
			return m_ChunksPtr[i - 1][j];
		return nullptr;
	}
	inline Chunk* GetRightChunk(int i, int j)
	{
		if (i != m_ChunksMatrixSize - 1)
			return m_ChunksPtr[i + 1][j];
		return nullptr;
	}
	inline Chunk* GetBackChunk(int i, int j)
	{
		if (j != 0)
			return m_ChunksPtr[i][j - 1];
		return nullptr;
	}
	inline Chunk* GetFrontChunk(int i, int j)
	{
		if (j != m_ChunksMatrixSize - 1)
			return m_ChunksPtr[i][j + 1];
		return nullptr;
	}

	//For constructor
	void Init();
public:
	World(const int& visibleRange = 3, const glm::ivec2& seed = glm::ivec2(0, 0));
	~World();
	void RemoveVoxelCamera(const Camera& cam);
	void PlaceVoxelCamera(const Camera& cam, const VoxelType& voxel);
	void UpdateChunksPos(const Camera& cam);
	void Draw(Camera& cam, const glm::vec4& lightPos, const float& lightDistance, const float& deltaTime);
	inline unsigned int GetMaxHeight(const glm::ivec2& pos)
	{
		int posX = 0;
		int posY = 0;
		bool find = false;
		for (posX = 0; posX < (int)m_ChunksMatrixSize && !find; posX++)
		{
			for (posY = 0; posY < (int)m_ChunksMatrixSize && !find; posY++)
			{
				if (m_ChunksPtr[posX][posY]->IsGlobalCoordInsideXZ({ pos.x, 0, pos.y }))
					find = true;
			}
		}
		if (find == false)
			return 256;

		--posX;
		--posY;
		
		for (int i = 254; i >= 0; --i)
			if (m_ChunksPtr[posX][posY]->GetVoxelTypeGlobal(pos.x, i, pos.y) != VoxelType::EMPTY)
				return i + 2;
		return 256;
	}
	void Recreate(const glm::ivec2& seed = glm::ivec2(0, 0));
};

