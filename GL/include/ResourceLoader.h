#pragma once
#include <vector>

namespace GLR
{
	class Mesh;

	class ResourceLoader
	{
	public:
		static void LoadMeshes(const char* file, std::vector<Mesh>& meshes);
	};

}