#pragma once

#include <map>
#include "glm/glm.hpp"
#include "Chunk.h"
#include <string>

class ChunkChanges
{
	//           Chunk pos                       Voxel pos
	//       PosX          PosZ          X             Y             Z
	std::map<int, std::map<int, std::map<int, std::map<int, std::map<int, VoxelType>>> > > m_ChunkChanges;
public:
	//For binary save/load
	bool LoadFromFile(const std::string& filepath);
	void SaveToFile(const std::string& filepath);

	//For text save/load
	bool LoadFromFileText(const std::string& filepath);
	void SaveToFileText(const std::string& filepath);

	void AddLocal(const glm::ivec2& ChunkPos, const glm::ivec3& coords, const VoxelType& voxel);
	void AddGlobal(const glm::ivec2& ChunkPos, const glm::ivec3& coords, const VoxelType& voxel);

	//Only if its need to change
	void ApplyChunkChanges(Chunk * chunk);
	void Clear();
};

