#include "World.h"

#include "opengl/Renderer.h"
#include "ChunkSize.h"

//TEMP
#include <iostream>
#include <thread>
#include <ctime>

#define FRUSTUM_CULLING_ENABLE

glm::ivec2 World::GetChunkPos(const glm::vec3 & pos)
{
	return floor(pos / float(MaxSize));
}

void World::RefillChunks()
{
	if (m_ChunksRefillNeeds[m_MidPos.x][m_MidPos.y] == true)
	{
		UpdateChunk(m_MidPos.x, m_MidPos.y);
		return;
	}

	bool first = true;
	if (m_IsUpdatePosStored == false)
	{
		m_UpdatePosLeftUp = m_VisibleRange - 1;
		first = false;
	}
	//int counter = 0;

	for (; m_UpdatePosLeftUp >= 0; --m_UpdatePosLeftUp)
	{
		unsigned int size = m_ChunksMatrixSize - m_UpdatePosLeftUp;

		for (unsigned int delta = m_UpdatePosLeftUp; delta < size; ++delta)
		{
			if (m_IsUpdatePosStored == true && first)
			{
				first = false;
				delta = m_UpdatePosDelta;
			}

			//std::cout << "PosLeftUp = " << m_UpdatePosLeftUp << "; Delta = " << delta << "; Stored delta = " << m_UpdatePosDelta << std::endl;

			if (m_ChunksRefillNeeds[delta][m_UpdatePosLeftUp] == true)
			{
				m_IsUpdatePosStored = true;
				m_UpdatePosDelta = delta;
				UpdateChunk(delta, m_UpdatePosLeftUp);
				//std::cout << "PosLeftUp = " << m_UpdatePosLeftUp << "; Delta = " << m_UpdatePosDelta << std::endl;
				//std::cout << "Updated Top" << std::endl;
				return;
				//counter++;
			}
			if (m_ChunksRefillNeeds[m_UpdatePosLeftUp][delta] == true)
			{
				m_IsUpdatePosStored = true;
				m_UpdatePosDelta = delta;
				UpdateChunk(m_UpdatePosLeftUp, delta);
				//std::cout << "PosLeftUp = " << m_UpdatePosLeftUp << "; Delta = " << m_UpdatePosDelta << std::endl;
				//std::cout << "Updated Left" << std::endl;

				return;
				//counter++;
			}

			if (m_ChunksRefillNeeds[m_ChunksMatrixSize - delta - 1][size - 1] == true)
			{
				m_IsUpdatePosStored = true;
				m_UpdatePosDelta = delta;
				UpdateChunk(m_ChunksMatrixSize - delta - 1, size - 1);
				//std::cout << "PosLeftUp = " << m_UpdatePosLeftUp << "; Delta = " << m_UpdatePosDelta << std::endl;
				//std::cout << "Updated Bottom" << std::endl;

				return;
				//counter++;
			}
			if (m_ChunksRefillNeeds[size  - 1][m_ChunksMatrixSize - delta - 1] == true)
			{
				m_IsUpdatePosStored = true;
				m_UpdatePosDelta = delta;
				UpdateChunk(size - 1, m_ChunksMatrixSize - delta - 1);
				//std::cout << "PosLeftUp = " << m_UpdatePosLeftUp << "; Delta = " << m_UpdatePosDelta << std::endl;
				//std::cout << "Updated Right" << std::endl;

				return;
				//counter++;
			}
			//if (counter >=4)
			//	return;
		}
	}
	
	
	//for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
	//	for (unsigned int j = 0; j < m_ChunksMatrixSize; j++)
	//	{
	//		if (m_ChunksRefillNeeds[i][j] == true)
	//		{
	//			UpdateChunk(i, j);
	//			return;
	//		}
	//	}


#ifdef FRUSTUM_CULLING_ENABLE
	//for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
	//	for (unsigned int j = 0; j < m_ChunksMatrixSize; j++)
	//		if (m_ChunksRefillNeeds[i][j] == true)
	//		{
	//			m_ChunksRefillNeeds[i][j] = false;				
	//			
	//			// for frustum culling if disable culling will laggs when chunks will be first time updated
	//			m_ChunksPtr[i][j]->ForceUpdate(GetLeftChunk(i, j), GetRightChunk(i, j), GetFrontChunk(i, j), GetBackChunk(i, j));
	//		}
#endif
	m_IsChunksNeedsReposition = false;
}


void World::UpdateChunk(const int i, const int j)
{
	m_ChunksRefillNeeds[i][j] = false;
	m_ChunksRefillNeedsBuffer[i][j] = false;
	m_ChunksPtr[i][j]->Refill({ i - m_VisibleRange + m_PlayerChunkPos.x, j - m_VisibleRange + m_PlayerChunkPos.y }, m_Seed);

	//Changes
	m_ChunkChanges.ApplyChunkChanges(m_ChunksPtr[i][j]);

	m_ChunksPtr[i][j]->ForceUpdate(GetLeftChunk(i, j), GetRightChunk(i, j), GetFrontChunk(i, j), GetBackChunk(i, j));
	if (i != 0)
		if (m_ChunksRefillNeeds[i - 1][j] == false)
			m_ChunksPtr[i - 1][j]->ForceUpdate(GetLeftChunk(i - 1, j), GetRightChunk(i - 1, j), GetFrontChunk(i - 1, j), GetBackChunk(i - 1, j));
	if (i != m_ChunksMatrixSize - 1)
		if (m_ChunksRefillNeeds[i + 1][j] == false)
			m_ChunksPtr[i + 1][j]->ForceUpdate(GetLeftChunk(i + 1, j), GetRightChunk(i + 1, j), GetFrontChunk(i + 1, j), GetBackChunk(i + 1, j));
	if (j != 0)
		if (m_ChunksRefillNeeds[i][j - 1] == false)
			m_ChunksPtr[i][j - 1]->ForceUpdate(GetLeftChunk(i, j - 1), GetRightChunk(i, j - 1), GetFrontChunk(i, j - 1), GetBackChunk(i, j - 1));
	if (j != m_ChunksMatrixSize - 1)
		if (m_ChunksRefillNeeds[i][j + 1] == false)
			m_ChunksPtr[i][j + 1]->ForceUpdate(GetLeftChunk(i, j + 1), GetRightChunk(i, j + 1), GetFrontChunk(i, j + 1), GetBackChunk(i, j + 1));
}

bool World::isPointInsideFrustum(const glm::vec3 & point, float frustum[6][4])
{
	for (int p = 0; p < 6; p++)
		if (frustum[p][0] * point.x + frustum[p][1] * point.y + frustum[p][2] * point.z + frustum[p][3] <= 0)
			return false;
	return true;
}

bool World::isChunkInsideFrustum(const Chunk * chunk, float frustum[6][4])
{
	glm::ivec2 pos = chunk->GetPos();
	for (int p = 0; p < 6; p++)
	{
		if (frustum[p][0] * float(pos.x * (int)MaxSize)                                        + frustum[p][2] * float(pos.y * (int)MaxSize)                + frustum[p][3] > 0)
			continue;							                                                   								            		        
		if (frustum[p][0] * float(pos.x * (int)MaxSize + (int)MaxSize)                         + frustum[p][2] * float(pos.y * (int)MaxSize)                + frustum[p][3] > 0)
			continue;																					            								        
		if (frustum[p][0] * float(pos.x * (int)MaxSize)			       + frustum[p][1] * 255.f + frustum[p][2] * float(pos.y * (int)MaxSize)                + frustum[p][3] > 0)
			continue;																																        
		if (frustum[p][0] * float(pos.x * (int)MaxSize + (int)MaxSize) + frustum[p][1] * 255.f + frustum[p][2] * float(pos.y * (int)MaxSize)                + frustum[p][3] > 0)
			continue;
		if (frustum[p][0] * float(pos.x * (int)MaxSize)                                        + frustum[p][2] * float(pos.y * (int)MaxSize + (int)MaxSize) + frustum[p][3] > 0)
			continue;							                                                   
		if (frustum[p][0] * float(pos.x * (int)MaxSize + (int)MaxSize)                         + frustum[p][2] * float(pos.y * (int)MaxSize + (int)MaxSize) + frustum[p][3] > 0)
			continue;
		if (frustum[p][0] * float(pos.x * (int)MaxSize)                + frustum[p][1] * 255.f + frustum[p][2] * float(pos.y * (int)MaxSize + (int)MaxSize) + frustum[p][3] > 0)
			continue;
		if (frustum[p][0] * float(pos.x * (int)MaxSize + (int)MaxSize) + frustum[p][1] * 255.f + frustum[p][2] * float(pos.y * (int)MaxSize + (int)MaxSize) + frustum[p][3] > 0)
			continue;

		return false;
	}
	return true;
}

bool World::FrustumCullingCheck(Camera & cam, const Chunk *chunk)
{
	glm::mat4 ViewProj = cam.GetViewProj();
	float frustum[6][4];

	/* Extract the numbers for the RIGHT plane */
	frustum[0][0] = ViewProj[0][3] - ViewProj[0][0];
	frustum[0][1] = ViewProj[1][3] - ViewProj[1][0];
	frustum[0][2] = ViewProj[2][3] - ViewProj[2][0];
	frustum[0][3] = ViewProj[3][3] - ViewProj[3][0];

	/* Normalize the result */
	float t = sqrt(frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2]);
	frustum[0][0] /= t;
	frustum[0][1] /= t;
	frustum[0][2] /= t;
	frustum[0][3] /= t;

	/* Extract the numbers for the LEFT plane */
	frustum[1][0] = ViewProj[0][3] + ViewProj[0][0];
	frustum[1][1] = ViewProj[1][3] + ViewProj[1][0];
	frustum[1][2] = ViewProj[2][3] + ViewProj[2][0];
	frustum[1][3] = ViewProj[3][3] + ViewProj[3][0];

	/* Normalize the result */
	t = sqrt(frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2]);
	frustum[1][0] /= t;
	frustum[1][1] /= t;
	frustum[1][2] /= t;
	frustum[1][3] /= t;

	/* Extract the BOTTOM plane */
	frustum[2][0] = ViewProj[0][3] + ViewProj[0][1];
	frustum[2][1] = ViewProj[1][3] + ViewProj[1][1];
	frustum[2][2] = ViewProj[2][3] + ViewProj[2][1];
	frustum[2][3] = ViewProj[3][3] + ViewProj[3][1];

	/* Normalize the result */
	t = sqrt(frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2]);
	frustum[2][0] /= t;
	frustum[2][1] /= t;
	frustum[2][2] /= t;
	frustum[2][3] /= t;

	/* Extract the TOP plane */
	frustum[3][0] = ViewProj[0][3] - ViewProj[0][1];
	frustum[3][1] = ViewProj[1][3] - ViewProj[1][1];
	frustum[3][2] = ViewProj[2][3] - ViewProj[2][1];
	frustum[3][3] = ViewProj[3][3] - ViewProj[3][1];

	/* Normalize the result */
	t = sqrt(frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2]);
	frustum[3][0] /= t;
	frustum[3][1] /= t;
	frustum[3][2] /= t;
	frustum[3][3] /= t;

	/* Extract the FAR plane */
	frustum[4][0] = ViewProj[0][3] - ViewProj[0][2];
	frustum[4][1] = ViewProj[1][3] - ViewProj[1][2];
	frustum[4][2] = ViewProj[2][3] - ViewProj[2][2];
	frustum[4][3] = ViewProj[3][3] - ViewProj[3][2];

	/* Normalize the result */
	t = sqrt(frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2]);
	frustum[4][0] /= t;
	frustum[4][1] /= t;
	frustum[4][2] /= t;
	frustum[4][3] /= t;

	/* Extract the NEAR plane */
	frustum[5][0] = ViewProj[0][3] + ViewProj[0][2];
	frustum[5][1] = ViewProj[1][3] + ViewProj[1][2];
	frustum[5][2] = ViewProj[2][3] + ViewProj[2][2];
	frustum[5][3] = ViewProj[3][3] + ViewProj[3][2];

	/* Normalize the result */
	t = sqrt(frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2]);
	frustum[5][0] /= t;
	frustum[5][1] /= t;
	frustum[5][2] /= t;
	frustum[5][3] /= t;

	//glm::ivec2 tmp = chunk->GetPos();
	//return isPointInsideFrustum(glm::vec3(tmp.x * 16.f, 127.5f, tmp.y * 16.f), frustum);
	return isChunkInsideFrustum(chunk, frustum);
}

void World::Init()
{
	unsigned int start_time = clock(); // Begin time
	m_BufferPtr = new Chunk**[m_ChunksMatrixSize];
	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
		m_BufferPtr[i] = new Chunk*[m_ChunksMatrixSize];

	m_ChunksPtr = new Chunk**[m_ChunksMatrixSize];
	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
	{
		m_ChunksPtr[i] = new Chunk*[m_ChunksMatrixSize];
		for (unsigned int j = 0; j < m_ChunksMatrixSize; j++)
		{
			m_ChunksPtr[i][j] = new Chunk({ int(i) - int(m_VisibleRange), int(j) - int(m_VisibleRange) }, m_Seed);
			m_BufferPtr[i][j] = m_ChunksPtr[i][j];
			m_ChunkChanges.ApplyChunkChanges(m_ChunksPtr[i][j]);

			//TEMP
			if ((((i)* m_ChunksMatrixSize + (j - 1)) * 100 / (m_ChunksMatrixSize * m_ChunksMatrixSize)) != ((i * m_ChunksMatrixSize + j) * 100 / (m_ChunksMatrixSize * m_ChunksMatrixSize)))
				std::cout << "Chunks loading " << (i * m_ChunksMatrixSize + j) * 100 / (m_ChunksMatrixSize * m_ChunksMatrixSize) << '%' << std::endl;
		}
	}
	//TEMP
	std::cout << "Chunks loading 100%" << std::endl;
	unsigned int end_time = clock(); // End time
	unsigned int search_time = end_time - start_time; // Time between
	std::cout << "Create chunks time = " << search_time << " ms" << std::endl; //Temp info

#ifdef FRUSTUM_CULLING_ENABLE
	// for frustum culling if disable culling will laggs when chunks will be first time updated
	start_time = clock(); // Begin time
	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
		for (unsigned int j = 0; j < m_ChunksMatrixSize; j++)
		{
			m_ChunksPtr[i][j]->ForceUpdate(GetLeftChunk(i, j), GetRightChunk(i, j), GetFrontChunk(i, j), GetBackChunk(i, j));

			//TEMP
			if ((((i)* m_ChunksMatrixSize + (j - 1)) * 100 / (m_ChunksMatrixSize * m_ChunksMatrixSize)) != ((i * m_ChunksMatrixSize + j) * 100 / (m_ChunksMatrixSize * m_ChunksMatrixSize)))
				std::cout << "Loading GPU memory " << (i * m_ChunksMatrixSize + j) * 100 / (m_ChunksMatrixSize * m_ChunksMatrixSize) << '%' << std::endl;
		}
	//TEMP
	std::cout << "Loading GPU memory 100%" << std::endl;
	end_time = clock(); // End time
	search_time = end_time - start_time; // Time between
	std::cout << "Loading GPU memory time = " << search_time << " ms" << std::endl; // Temp info
#endif

	m_ChunksRefillNeeds = new bool*[m_ChunksMatrixSize];
	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
		m_ChunksRefillNeeds[i] = new bool[m_ChunksMatrixSize] {true};

	m_ChunksRefillNeedsBuffer = new bool*[m_ChunksMatrixSize];
	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
		m_ChunksRefillNeedsBuffer[i] = new bool[m_ChunksMatrixSize] {true};

	m_IsInsideFrustum = new bool*[m_ChunksMatrixSize];
	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
		m_IsInsideFrustum[i] = new bool[m_ChunksMatrixSize] {false};
}

World::World(const int & visibleRange, const glm::ivec2 & seed)
	: m_VisibleRange(visibleRange), m_Seed(seed), m_ChunksMatrixSize(visibleRange * 2 + 1), m_Shader("res/shaders/FaceChunk.shader"),
	m_IsChunksPosUpdated(true), m_MidPos(visibleRange), m_PlayerChunkPos(0),
	m_GrassTexture("res/textures/Grass.jpg"),
	m_WaterTexture("res/textures/WaterAlpha.png"),
	m_SandTexture("res/textures/Sand.png"),
	m_DirtTexture("res/textures/Dirt.png"),
	m_SnowTexture("res/textures/Snow.png"),
	m_IsChunksNeedsReposition(false), m_WaterTime(0.f), m_IsUpdatePosStored(false),
	m_UpdatePosLeftUp(0), m_UpdatePosDelta(0)
{
	m_ChunkChanges.LoadFromFile("changes.data");
	Init();
}

World::~World()
{
	m_ChunkChanges.SaveToFile("changes.data");
	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
	{
		for (unsigned int j = 0; j < m_ChunksMatrixSize; j++)
		{
			delete m_ChunksPtr[i][j];
		}
		delete[] m_ChunksPtr[i];
	}
	delete[] m_ChunksPtr;

	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
		delete[] m_BufferPtr[i];
	delete[] m_BufferPtr;

	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
		delete[] m_ChunksRefillNeeds[i];
	delete[] m_ChunksRefillNeeds;

	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
		delete[] m_ChunksRefillNeedsBuffer[i];
	delete[] m_ChunksRefillNeedsBuffer;

	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
		delete[] m_IsInsideFrustum[i];
	delete[] m_IsInsideFrustum;
}

void World::RemoveVoxelCamera(const Camera & cam)
{
	float distance = 90.1f;
	glm::vec3 f = cam.GetFront();
	glm::vec3 p = cam.GetPos();
	float offset = 0.f;
	float step = 0.1f;

	bool stop = false;
	while (offset < distance)
	{
		glm::vec3 tmp = p + f * offset;
		int x = (int)floor(tmp.x);
		int y = (int)floor(tmp.y);
		int z = (int)floor(tmp.z);

		int i = 0;
		int j = 0;
		if (!m_ChunksPtr[i][j]->IsGlobalCoordInside({ x, y, z }))
		{
			bool find = false;
			for (i = 0; i < (int)m_ChunksMatrixSize && !find; i++)
			{
				for (j = 0; j < (int)m_ChunksMatrixSize && !find; j++)
				{
					if (m_ChunksPtr[i][j]->IsGlobalCoordInside({ x, y, z }))
						find = true;
				}
			}
			if (find == false)
			{
				offset += step;
				continue;
			}
			--i;
			--j;
		}
		
		//if (m_ChunksPtr[i][j]->GetVoxelTypeGlobal(x, y, z) != VoxelType::EMPTY && m_ChunksPtr[i][j]->GetVoxelTypeGlobal(x, y, z) != VoxelType::WATER) //water is undeleteable
		if (m_ChunksPtr[i][j]->GetVoxelTypeGlobal(x, y, z) != VoxelType::EMPTY) //water is solid
		{
			m_ChunksPtr[i][j]->SetDataGlobalCoords(x, y, z, VoxelType::EMPTY, GetLeftChunk(i, j), GetRightChunk(i, j), GetFrontChunk(i, j), GetBackChunk(i, j));

			//Saving changes
			m_ChunkChanges.AddGlobal(m_ChunksPtr[i][j]->m_Pos, { x, y, z }, VoxelType::EMPTY);

			
			break;
		}

		
		offset += step;
	}
}

void World::PlaceVoxelCamera(const Camera & cam, const VoxelType& voxel)
{
	float distance = 90.1f;
	glm::vec3 f = cam.GetFront();
	glm::vec3 p = cam.GetPos();
	float offset = 0.f;
	float step = 0.1f;

	int i = 0;
	int j = 0;
	int oldi = 0;
	int oldj = 0;
	while (offset < distance)
	{
		glm::vec3 tmp = p + f * offset;
		int x = (int)floor(tmp.x);
		int y = (int)floor(tmp.y);
		int z = (int)floor(tmp.z);

		if (!m_ChunksPtr[i][j]->IsGlobalCoordInside({ x, y, z }))
		{
			oldi = i;
			oldj = j;
			i = 0;
			j = 0;
			bool find = false;
			for (i = 0; i < (int)m_ChunksMatrixSize && !find; i++)
			{
				for (j = 0; j < (int)m_ChunksMatrixSize && !find; j++)
				{
					if (m_ChunksPtr[i][j]->IsGlobalCoordInside({ x, y, z }))
						find = true;
				}
			}
			if (find == false)
			{
				offset += step;
				i = 0;
				j = 0;
				continue;
			}
			--i;
			--j;
		}

		//if (m_ChunksPtr[i][j]->GetVoxelTypeGlobal(x, y, z) != VoxelType::EMPTY && m_ChunksPtr[i][j]->GetVoxelTypeGlobal(x, y, z) != VoxelType::WATER) //water is like empty
		if (m_ChunksPtr[i][j]->GetVoxelTypeGlobal(x, y, z) != VoxelType::EMPTY) //whater is solid
		{
			glm::vec3 tmp2 = p + f * (offset - step);
			int plx = (int)floor(tmp2.x);
			int ply = (int)floor(tmp2.y);
			int plz = (int)floor(tmp2.z);

			if (plx == (int)floor(p.x) && ply == (int)floor(p.y) && plz == (int)floor(p.z)) // if tries place voxel at camera pos
				break;
			if (m_ChunksPtr[i][j]->IsGlobalCoordInside({ plx, ply, plz }))
			{
				m_ChunksPtr[i][j]->SetDataGlobalCoords(plx, ply, plz, voxel, GetLeftChunk(i, j), GetRightChunk(i, j), GetFrontChunk(i, j), GetBackChunk(i, j));

				//Saving changes
				m_ChunkChanges.AddGlobal(m_ChunksPtr[i][j]->m_Pos, { plx, ply, plz }, voxel);
			}
			else if (m_ChunksPtr[oldi][oldj]->IsGlobalCoordInside({ plx, ply, plz }))
			{
				m_ChunksPtr[oldi][oldj]->SetDataGlobalCoords(plx, ply, plz, voxel, GetLeftChunk(oldi, oldj), GetRightChunk(oldi, oldj), GetFrontChunk(oldi, oldj), GetBackChunk(oldi, oldj));

				//Saving changes
				m_ChunkChanges.AddGlobal(m_ChunksPtr[oldi][oldj]->m_Pos, { plx, ply, plz }, voxel);
			}

			break;
		}
		/*
		if (!m_ChunksPtr[m_MidPos.x][m_MidPos.y]->IsGlobalCoordInside({x, y, z}))
		{
			offset += step;
			continue;
		}
		if (m_ChunksPtr[m_MidPos.x][m_MidPos.y]->GetVoxelTypeGlobal(x, y, z) != VoxelType::EMPTY)
		{
			glm::vec3 tmp2 = p + f * (offset - step);
			int plx = (int)floor(tmp2.x);
			int ply = (int)floor(tmp2.y);
			int plz = (int)floor(tmp2.z);
			if (!m_ChunksPtr[m_MidPos.x][m_MidPos.y]->IsGlobalCoordInside({ plx, ply, plz }))
				break;

			m_ChunksPtr[m_MidPos.x][m_MidPos.y]->SetDataGlobalCoords(plx, ply, plz, VoxelType::GRASS);
			break;
		}
		*/
		offset += step;
	}
}

void World::UpdateChunksPos(const Camera & cam)
{
	m_IsChunksPosUpdated = false;

	glm::ivec2 NewPlayerPos = glm::ivec2(floor(cam.GetPos().x / float(MaxSize)), floor(cam.GetPos().z / float(MaxSize)));
	glm::ivec2 PlayerMove = NewPlayerPos - m_PlayerChunkPos;
	std::cout << "Current Chunk = {" << NewPlayerPos.x << ", " << NewPlayerPos.y << "}; ";
	std::cout << "Move Coords = {" << PlayerMove.x << ", " << PlayerMove.y << "}" << std::endl;


	for (int i = 0; i < (int)m_ChunksMatrixSize; i++)
	{
		for (int j = 0; j < (int)m_ChunksMatrixSize; j++)
		{
			if (i + PlayerMove.x < (int)m_ChunksMatrixSize && i + PlayerMove.x >= 0 &&
				j + PlayerMove.y < (int)m_ChunksMatrixSize && j + PlayerMove.y >= 0)
			{
				m_ChunksPtr[i][j] = m_BufferPtr[i + PlayerMove.x][j + PlayerMove.y];

				//m_ChunksRefillNeeds[i][j] = true;
				m_ChunksRefillNeeds[i][j] = m_ChunksRefillNeedsBuffer[i + PlayerMove.x][j + PlayerMove.y];
			}
			else
			{
				m_ChunksPtr[i][j] = m_BufferPtr[m_ChunksMatrixSize - i - 1][m_ChunksMatrixSize - j - 1];

				//NEW
				m_ChunksRefillNeeds[i][j] = true;
				m_IsChunksNeedsReposition = true;

				//OLD
				//m_ChunksPtr[i][j]->Refill({ i - m_VisibleRange + NewPlayerPos.x, j - m_VisibleRange + NewPlayerPos.y }, m_Seed);
			}
				
		}
	}

	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
		for (unsigned int j = 0; j < m_ChunksMatrixSize; j++)
		{
			m_BufferPtr[i][j] = m_ChunksPtr[i][j];
			m_ChunksRefillNeedsBuffer[i][j] = m_ChunksRefillNeeds[i][j];
		}

	m_PlayerChunkPos = NewPlayerPos;
	m_IsUpdatePosStored = false;
}

void World::Draw(Camera & cam, const glm::vec4& lightPos, const float& lightDistance, const float& deltaTime)
{
	if (!m_ChunksPtr[m_MidPos.x][m_MidPos.y]->IsGlobalCoordInsideXZ(cam.GetPos()))
		m_IsChunksPosUpdated = true;

	if (m_IsChunksPosUpdated)
		UpdateChunksPos(cam);
	//{
		//std::thread t1(&World::UpdateChunksPos, this, cam);
		//t1.detach();
	//}

	if (m_IsChunksNeedsReposition)
		RefillChunks();

	m_GrassTexture.Bind(0);
	m_WaterTexture.Bind(1);
	m_SandTexture.Bind(2);
	m_DirtTexture.Bind(3);
	m_SnowTexture.Bind(4);

	m_Shader.Bind();
	m_Shader.SetUniform1i("u_TextureGrass", 0);
	m_Shader.SetUniform1i("u_TextureWater", 1);
	m_Shader.SetUniform1i("u_TextureSand", 2);
	m_Shader.SetUniform1i("u_TextureDirt", 3);
	m_Shader.SetUniform1i("u_TextureSnow", 4);

	m_Shader.SetUniformMatrix4f("u_ViewProj", cam.GetViewProj());
	m_Shader.SetUniformVec4f("u_posLight", lightPos);
	m_Shader.SetUniform1f("u_LightDistance", lightDistance);

	m_WaterTime += deltaTime;
	if (m_WaterTime > 3.14f * 2.f)
		m_WaterTime = 0;
	m_Shader.SetUniform1f("u_WaterWave", m_WaterTime);

	//Draw nonTransparent voxels
	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
	{
		for (unsigned int j = 0; j < m_ChunksMatrixSize; j++)
		{
#ifdef FRUSTUM_CULLING_ENABLE
			if (!FrustumCullingCheck(cam, m_ChunksPtr[i][j]) || m_ChunksRefillNeeds[i][j])
			{
				m_IsInsideFrustum[i][j] = false;
				continue;
			}
			m_IsInsideFrustum[i][j] = true;
#endif

			m_Shader.SetUniformMatrix4f("u_Model", glm::translate(glm::mat4(1), glm::vec3(m_ChunksPtr[i][j]->GetPos().x * int(MaxSize), 0.f, m_ChunksPtr[i][j]->GetPos().y * int(MaxSize))));
			m_ChunksPtr[i][j]->Draw(GetLeftChunk(i, j), GetRightChunk(i, j), GetFrontChunk(i, j), GetBackChunk(i, j));
		}
	}
	
	//Simple unordered transparent draw
	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
	{
		for (unsigned int j = 0; j < m_ChunksMatrixSize; j++)
		{
#ifdef FRUSTUM_CULLING_ENABLE
			if (!m_IsInsideFrustum[i][j])
				continue;
#endif
			m_Shader.SetUniformMatrix4f("u_Model", glm::translate(glm::mat4(1), glm::vec3(m_ChunksPtr[i][j]->GetPos().x * int(MaxSize), 0.f, m_ChunksPtr[i][j]->GetPos().y * int(MaxSize))));
			
			//No needs to update because we already update all in Draw
			m_ChunksPtr[i][j]->DrawTransparent();
		}
	}
	

	/*
	//DrawWater from near to far
	if (m_IsInsideFrustum[m_MidPos.x][m_MidPos.y])
	{
		m_Shader.SetUniformMatrix4f("u_Model", glm::translate(glm::mat4(1), glm::vec3(m_ChunksPtr[m_MidPos.x][m_MidPos.y]->GetPos().x * int(MaxSize),
									0.f, m_ChunksPtr[m_MidPos.x][m_MidPos.y]->GetPos().y * int(MaxSize))));
		m_ChunksPtr[m_MidPos.x][m_MidPos.y]->DrawTransparent();
	}

	for (int l = 1; l <= m_VisibleRange; l++)
	{
		glm::vec3 tmp;
		Chunk *chptr;
		for (int i = 0; i <= l; i++)
		{
			//Clockvise
			//Top
			if (m_IsInsideFrustum[m_MidPos.x + i][m_MidPos.y - l])
			{
				chptr = m_ChunksPtr[m_MidPos.x + i][m_MidPos.y - l];
				tmp = glm::vec3(chptr->GetPos().x * int(MaxSize), 0.f, chptr->GetPos().y * int(MaxSize));
				m_Shader.SetUniformMatrix4f("u_Model", glm::translate(glm::mat4(1), tmp));
				chptr->DrawTransparent();
			}

			//Right
			if (m_IsInsideFrustum[m_MidPos.x + l][m_MidPos.y + i])
			{
				chptr = m_ChunksPtr[m_MidPos.x + l][m_MidPos.y + i];
				tmp = glm::vec3(chptr->GetPos().x * int(MaxSize), 0.f, chptr->GetPos().y * int(MaxSize));
				m_Shader.SetUniformMatrix4f("u_Model", glm::translate(glm::mat4(1), tmp));
				chptr->DrawTransparent();
			}
			//Bottom
			if (m_IsInsideFrustum[m_MidPos.x - i][m_MidPos.y + l])
			{
				chptr = m_ChunksPtr[m_MidPos.x - i][m_MidPos.y + l];
				tmp = glm::vec3(chptr->GetPos().x * int(MaxSize), 0.f, chptr->GetPos().y * int(MaxSize));
				m_Shader.SetUniformMatrix4f("u_Model", glm::translate(glm::mat4(1), tmp));
				chptr->DrawTransparent();
			}
			//Left
			if (m_IsInsideFrustum[m_MidPos.x - l][m_MidPos.y - i])
			{
				chptr = m_ChunksPtr[m_MidPos.x - l][m_MidPos.y - i];
				tmp = glm::vec3(chptr->GetPos().x * int(MaxSize), 0.f, chptr->GetPos().y * int(MaxSize));
				m_Shader.SetUniformMatrix4f("u_Model", glm::translate(glm::mat4(1), tmp));
				chptr->DrawTransparent();
			}
			//ReverseClockvise
			if (i != l && i != 0)
			{
				if (m_IsInsideFrustum[m_MidPos.x - i][m_MidPos.y - l])
				{
					chptr = m_ChunksPtr[m_MidPos.x - i][m_MidPos.y - l];
					tmp = glm::vec3(chptr->GetPos().x * int(MaxSize), 0.f, chptr->GetPos().y * int(MaxSize));
					m_Shader.SetUniformMatrix4f("u_Model", glm::translate(glm::mat4(1), tmp));
					chptr->DrawTransparent();
				}

				//Right
				if (m_IsInsideFrustum[m_MidPos.x + l][m_MidPos.y - i])
				{
					chptr = m_ChunksPtr[m_MidPos.x + l][m_MidPos.y - i];
					tmp = glm::vec3(chptr->GetPos().x * int(MaxSize), 0.f, chptr->GetPos().y * int(MaxSize));
					m_Shader.SetUniformMatrix4f("u_Model", glm::translate(glm::mat4(1), tmp));
					chptr->DrawTransparent();
				}
				//Bottom
				if (m_IsInsideFrustum[m_MidPos.x + i][m_MidPos.y + l])
				{
					chptr = m_ChunksPtr[m_MidPos.x + i][m_MidPos.y + l];
					tmp = glm::vec3(chptr->GetPos().x * int(MaxSize), 0.f, chptr->GetPos().y * int(MaxSize));
					m_Shader.SetUniformMatrix4f("u_Model", glm::translate(glm::mat4(1), tmp));
					chptr->DrawTransparent();
				}
				//Left
				if (m_IsInsideFrustum[m_MidPos.x - l][m_MidPos.y + i])
				{
					chptr = m_ChunksPtr[m_MidPos.x - l][m_MidPos.y + i];
					tmp = glm::vec3(chptr->GetPos().x * int(MaxSize), 0.f, chptr->GetPos().y * int(MaxSize));
					m_Shader.SetUniformMatrix4f("u_Model", glm::translate(glm::mat4(1), tmp));
					chptr->DrawTransparent();
				}
			}
		}
	}
	*/
}

void World::Recreate(const glm::ivec2 & seed)
{
	m_Seed = seed;
	m_ChunkChanges.Clear();
	for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
		for (unsigned int j = 0; j < m_ChunksMatrixSize; j++)
			m_ChunksRefillNeeds[i][j] = m_ChunksRefillNeedsBuffer[i][j] = true;
			//m_ChunksPtr[i][j]->Refill({ int(i) - int(m_VisibleRange), int(j) - int(m_VisibleRange) }, m_Seed);
	//for (unsigned int i = 0; i < m_ChunksMatrixSize; i++)
	//	for (unsigned int j = 0; j < m_ChunksMatrixSize; j++)
	//		m_ChunksPtr[i][j]->ForceUpdate(GetLeftChunk(i, j), GetRightChunk(i, j), GetFrontChunk(i, j), GetBackChunk(i, j));

	m_IsChunksPosUpdated = true;
}
