#pragma once
#include "PhysicsShapes.h"

namespace GLR
{
	struct BVHFlatNode;

	class BVH
	{
	public:
		BVH(std::vector<BBox>& shapes);
		BVH(std::vector<BBox>& shapes, char* data, unsigned elementSize);
		~BVH();

		std::vector<unsigned> Intersect(const glm::vec3& point) const;
		BBox Get(unsigned index) const;
		const BVHFlatNode* GetTree() const;
		unsigned GetTreeSize() const;
		unsigned GetTreeSizeBytes() const;

	private:
		void Build(std::vector<BBox>& shapes, char* data = nullptr, unsigned elementSize = 0);

		unsigned m_nNodes, m_leafSize, m_nLeafs;
		std::vector<BVHFlatNode> m_flatTree;
	};
}
