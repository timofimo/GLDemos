#pragma once
#include <vector>
#include "PhysicsShapes.h"

namespace GLR
{
	struct BVHFlatNode;

	class BVH
	{
	public:
		BVH(std::vector<BBox>& shapes, unsigned leafSize);
		~BVH();

		std::vector<unsigned> Intersect(const glm::vec3& point) const;
		BBox Get(unsigned index) const;

	private:
		void Build(std::vector<BBox>& shapes);

		unsigned m_nNodes, m_leafSize, m_nLeafs;
		BVHFlatNode* m_flatTree;
	};
}
