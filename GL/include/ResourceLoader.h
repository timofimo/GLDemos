#pragma once

namespace GLR
{
	class Mesh;
	class MeshBuffer;
	class Material;

	enum class EBatchType
	{
		None, // No batching
		PerFile, // Batch all compatible meshes in the file
	};

	class ResourceLoader
	{
	public:
		static void LoadScene(const char* file, std::vector<Mesh>& meshes, EBatchType batchType, std::vector<Material>& materials, std::vector<unsigned>& materialIndices);
		static void LoadMeshes(const char* file, std::vector<Mesh>& meshes, EBatchType batchType);
		static void LoadMaterials(const char* file, std::vector<Material>& materials);

	private:
		static void LoadMeshes(const void* aiscene, std::vector<Mesh>& meshes, EBatchType batchType, std::vector<unsigned>& materialIndices);
		static void LoadMaterials(const void* aiscene, const char* file, std::vector<Material>& materials);

		static std::shared_ptr<MeshBuffer> m_currentMeshBuffer;	// Used for batching
	};

}