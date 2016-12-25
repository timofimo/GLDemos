#include "..\include\BVH.h"

struct BVHBuildEntry 
{
	unsigned parent;
	unsigned start, end;
};

struct GLR::BVHFlatNode 
{
	unsigned start;
	BBox bbox;
	unsigned rightOffset;
};

GLR::BVH::BVH(std::vector<BBox>& shapes) : m_nNodes(0), m_leafSize(1), m_nLeafs(0)
{
	Build(shapes);
}

GLR::BVH::BVH(std::vector<BBox>& shapes, char* data, unsigned elementSize) : m_nNodes(0), m_leafSize(1), m_nLeafs(0)
{
	Build(shapes, data, elementSize);
}

GLR::BVH::~BVH()
{
}

std::vector<unsigned> GLR::BVH::Intersect(const glm::vec3& point) const
{
	std::vector<unsigned> result;

	if (m_flatTree.empty())
		return result;

	unsigned todo[64];
	int stackptr = 0;

	todo[stackptr] = 0;

	while(stackptr >= 0)
	{
		unsigned ni = todo[stackptr];
		--stackptr;
		const BVHFlatNode& node(m_flatTree[ni]);

		if(node.rightOffset == 0)
		{
			if (node.bbox.Contains(point))
				result.push_back(ni);
		}
		else
		{
			if (m_flatTree[ni + 1].bbox.Contains(point))
				todo[++stackptr] = ni + 1;
			if (m_flatTree[ni + node.rightOffset].bbox.Contains(point))
				todo[++stackptr] = ni + node.rightOffset;
		}
	}

	return result;
}

GLR::BBox GLR::BVH::Get(unsigned index) const
{
	if (m_flatTree.empty())
		return BVHFlatNode().bbox;

	return m_flatTree[index].bbox;
}

const GLR::BVHFlatNode* GLR::BVH::GetTree() const
{
	return m_flatTree.data();
}

unsigned GLR::BVH::GetTreeSize() const
{
	return unsigned(m_flatTree.size());
}

unsigned GLR::BVH::GetTreeSizeBytes() const
{
	return unsigned(m_flatTree.size()) * sizeof(BVHFlatNode);
}

void GLR::BVH::Build(std::vector<BBox>& shapes, char* data, unsigned elementSize)
{
	char* tempElement = new char[elementSize];

	BVHBuildEntry todo[128];
	unsigned stackptr = 0;
	const unsigned untouched = 0xffffffff;
	const unsigned touchedTwice = 0xfffffffd;
	const unsigned rootIdentifier = 0xfffffffc;

	todo[stackptr].start = 0;
	todo[stackptr].end = unsigned(shapes.size());
	todo[stackptr].parent = rootIdentifier;
	++stackptr;

	BVHFlatNode node;
	m_flatTree.resize(shapes.size() * 2);
	unsigned currentBuildNodeIndex = 0;

	while(stackptr > 0)
	{
		BVHBuildEntry &bnode(todo[--stackptr]);
		unsigned start = bnode.start;
		unsigned end = bnode.end;
		unsigned nShapes = end - start;

		++m_nNodes;
		node.start = start;
		node.rightOffset = untouched;

		BBox bb(shapes[start]);
		BBox bc(shapes[start].Center());
		for(unsigned i = start + 1; i < end; i++)
		{
			bb.ExpandToInclude(shapes[i]);
			bc.ExpandToInclude(shapes[i].Center());
		}
		node.bbox = bb;

		if (nShapes <= m_leafSize)
		{
			node.rightOffset = 0;
			++m_nLeafs;
		}

		m_flatTree[currentBuildNodeIndex++] = node;

		if(bnode.parent != rootIdentifier)
		{
			--m_flatTree[bnode.parent].rightOffset;

			if (m_flatTree[bnode.parent].rightOffset == touchedTwice)
				m_flatTree[bnode.parent].rightOffset = m_nNodes - 1 - bnode.parent;
		}

		if(node.rightOffset == 0)
			continue;

		unsigned splitDimension = bc.MaxDimension();

		float splitCoord = 0.5f * (bc.Min()[splitDimension] + bc.Max()[splitDimension]);

		unsigned mid = start;
		for(unsigned i = start; i < end; i++)
		{
			if(shapes[i].Center()[splitDimension] < splitCoord)
			{
				if (data && elementSize)
				{
					memcpy(tempElement, &data[i * elementSize], elementSize);
					memcpy(&data[i * elementSize], &data[mid * elementSize], elementSize);
					memcpy(&data[mid * elementSize], tempElement, elementSize);
				}

				std::swap(shapes[i], shapes[mid]);
				++mid;
			}
		}

		if (mid == start || mid == end)
			mid = start + (end - start) / 2;

		todo[stackptr].start = mid;
		todo[stackptr].end = end;
		todo[stackptr].parent = m_nNodes - 1;
		++stackptr;

		todo[stackptr].start = start;
		todo[stackptr].end = mid;
		todo[stackptr].parent = m_nNodes - 1;
		++stackptr;
	}

	delete[] tempElement;
}
