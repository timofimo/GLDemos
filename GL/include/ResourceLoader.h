#pragma once
#include <vector>
#include <memory>

namespace GLR
{
	class Mesh;
	class MeshBuffer;

	enum class EBatchType
	{
		None, // No batching
		PerFile, // Batch all compatible meshes in the file
	};

	class ResourceLoader
	{
	public:
		static void LoadMeshes(const char* file, std::vector<Mesh>& meshes, EBatchType batchType);

	private:
		static std::shared_ptr<MeshBuffer> m_currentMeshBuffer;	// Used for batching
	};

}