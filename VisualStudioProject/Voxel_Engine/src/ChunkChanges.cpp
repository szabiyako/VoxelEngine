#include "ChunkChanges.h"

#include <fstream>

bool ChunkChanges::LoadFromFile(const std::string & filepath)
{
	std::ifstream in(filepath, std::ios::binary);
	if (!in)
		return false;

	struct Data
	{
		int PosX;
		int PosY;

		int X;
		int Y;
		int Z;

		unsigned char Voxel;
	};

	while (!in.eof())
	{
		Data tmp;

		in.read((char *)&(tmp.PosX), sizeof(int));
		in.read((char *)&(tmp.PosY), sizeof(int));
		in.read((char *)&(tmp.X), sizeof(int));
		in.read((char *)&(tmp.Y), sizeof(int));
		in.read((char *)&(tmp.Z), sizeof(int));
		in.read((char *)&(tmp.Voxel), sizeof(unsigned char));

		AddLocal({ tmp.PosX, tmp.PosY }, { tmp.X, tmp.Y, tmp.Z }, (VoxelType)tmp.Voxel);
	}
	return true;
}

void ChunkChanges::SaveToFile(const std::string & filepath)
{
	std::ofstream out(filepath, std::ios::binary);
	if (m_ChunkChanges.empty())
		return;
	for (auto const& PosX : m_ChunkChanges)
		for (auto const& PosY : PosX.second)
			for (auto const& X : PosY.second)
				for (auto const& Y : X.second)
					for (auto const& Z : Y.second)
					{
						out.write((char *)&(PosX.first), sizeof(int));
						out.write((char *)&(PosY.first), sizeof(int));
						out.write((char *)&(X.first), sizeof(int));
						out.write((char *)&(Y.first), sizeof(int));
						out.write((char *)&(Z.first), sizeof(int));
						out.write((char *)&(Z.second), sizeof(unsigned char));
					}
}

bool ChunkChanges::LoadFromFileText(const std::string& filepath)
{
	std::ifstream in(filepath);
	if (!in)
		return false;
	while (!in.eof())
	{
		glm::ivec2 Pos;
		glm::ivec3 Coord;
		int Voxel;

		char c;

		do in.get(c); while (c != '[' && !in.eof());
		if (in.eof())
			break;
		in >> Pos.x;

		do in.get(c); while (c != ',' && !in.eof());
		if (in.eof())
			break;
		in >> Pos.y;

		do in.get(c); while (c != '{' && !in.eof());
		if (in.eof())
			break;
		in >> Coord.x;

		do in.get(c); while (c != ',' && !in.eof());
		if (in.eof())
			break;
		in >> Coord.y;

		do in.get(c); while (c != ',' && !in.eof());
		if (in.eof())
			break;
		in >> Coord.z;

		do in.get(c); while (c != '=' && !in.eof());
		if (in.eof())
			break;
		in >> Voxel;

		AddLocal(Pos, Coord, (VoxelType)Voxel);
	}
	return true;
}

void ChunkChanges::SaveToFileText(const std::string& filepath)
{
	std::ofstream out(filepath);
	if (m_ChunkChanges.empty())
		return;
	for (auto const& PosX : m_ChunkChanges)
		for (auto const& PosY : PosX.second)
			for (auto const& X : PosY.second)
				for (auto const& Y : X.second)
					for (auto const& Z : Y.second)
						out << "[" << PosX.first << ", " << PosY.first << "]:{" << X.first << ", " << Y.first << ", " << Z.first << "} = " << Z.second << std::endl;
}

void ChunkChanges::AddLocal(const glm::ivec2& ChunkPos, const glm::ivec3& coords, const VoxelType& voxel)
{
	auto PosX = m_ChunkChanges.find(ChunkPos.x);
	if (PosX == m_ChunkChanges.end()) // if didnt find - create new
	{
		std::map<int, VoxelType> tmp0 = { {coords.z, voxel} };
		std::map<int, std::map<int, VoxelType>> tmp1 = { {coords.y, tmp0} };
		std::map<int, std::map<int, std::map<int, VoxelType>>> tmp2 = { {coords.x, tmp1} };
		std::map<int, std::map<int, std::map<int, std::map<int, VoxelType>>>> tmp3 = { {ChunkPos.y, tmp2} };
		m_ChunkChanges.insert_or_assign(ChunkPos.x, tmp3);
	}
	else // if found PosX, then search for PosY
	{
		auto PosY = PosX->second.find(ChunkPos.y);
		if (PosY == PosX->second.end()) // if didnt find - create new
		{
			std::map<int, VoxelType> tmp0 = { {coords.z, voxel} };
			std::map<int, std::map<int, VoxelType>> tmp1 = { {coords.y, tmp0} };
			std::map<int, std::map<int, std::map<int, VoxelType>>> tmp2 = { {coords.x, tmp1} };
			PosX->second.insert_or_assign(ChunkPos.y, tmp2);
		}
		else // if found PosY, then search for X
		{
			auto X = PosY->second.find(coords.x);
			if (X == PosY->second.end()) // if didnt find - create new
			{
				std::map<int, VoxelType> tmp0 = { {coords.z, voxel} };
				std::map<int, std::map<int, VoxelType>> tmp1 = { {coords.y, tmp0} };
				PosY->second.insert_or_assign(coords.x, tmp1);
			}
			else // if found X, then search for Y
			{
				auto Y = X->second.find(coords.y);
				if (Y == X->second.end()) // if didnt find - create new
				{
					std::map<int, VoxelType> tmp0 = { {coords.z, voxel} };
					X->second.insert_or_assign(coords.y, tmp0);
				}
				else // if found Y, then search for Z
				{
					auto Z = Y->second.find(coords.z);
					if (Z == Y->second.end()) // if didnt find - create new
					{
						Y->second.insert_or_assign(coords.z, voxel);
					}
					else // if found Z, then set Voxel
					{
						Y->second.insert_or_assign(coords.z, voxel);
					}
				}
			}
		}
	}
}
void ChunkChanges::AddGlobal(const glm::ivec2& ChunkPos, const glm::ivec3& coords, const VoxelType& voxel)
{
	AddLocal(ChunkPos, { coords.x - ChunkPos.x * 16, coords.y, coords.z - ChunkPos.y * 16 }, voxel);
}

void ChunkChanges::ApplyChunkChanges(Chunk * chunk)
{

	auto PosX = m_ChunkChanges.find(chunk->m_Pos.x);
	if (PosX == m_ChunkChanges.end())
		return;
	else // if found PosX, then search for PosY
	{
		auto PosY = PosX->second.find(chunk->m_Pos.y);
		if (PosY == PosX->second.end()) // if didnt find - create new
			return;
		else // if found PosY, then start
		{
			for (auto const& X : PosY->second)
				for (auto const& Y : X.second)
					for (auto const& Z : Y.second)
						chunk->m_Data[X.first][Y.first][Z.first] = Z.second;
		}
	}
}

void ChunkChanges::Clear()
{
	m_ChunkChanges.clear();
}

