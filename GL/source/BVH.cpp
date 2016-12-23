#include "..\include\BVH.h"

struct BVHBuildEntry 
{
	unsigned parent;
	unsigned start, end;
};

struct GLR::BVHFlatNode 
{
	BBox bbox;
	unsigned start, nShapes, rightOffset;
};

struct BVHTraversal 
{
	unsigned i;
	BVHTraversal() { }
	BVHTraversal(int _i) : i(_i) { }
};

GLR::BVH::BVH(std::vector<BBox>& shapes, unsigned leafSize) : m_nNodes(0), m_leafSize(leafSize), m_nLeafs(0), m_flatTree(nullptr)
{
	Build(shapes);
}

GLR::BVH::~BVH()
{
	delete[] m_flatTree;
}

std::vector<unsigned> GLR::BVH::Intersect(const glm::vec3& point) const
{
	std::vector<unsigned> result;

	if (!m_flatTree)
		return result;

	BVHTraversal todo[64];
	int stackptr = 0;

	todo[stackptr].i = 0;

	while(stackptr >= 0)
	{
		int ni = todo[stackptr].i;
		--stackptr;
		const BVHFlatNode& node(m_flatTree[ni]);

		if(node.rightOffset == 0)
		{
			for(unsigned i = 0; i < node.nShapes; i++)
			{
				if (node.bbox.Contains(point))
					result.push_back(ni);
			}
		}
		else
		{
			if (m_flatTree[ni + 1].bbox.Contains(point))
				todo[++stackptr] = BVHTraversal(ni + 1);
			if (m_flatTree[ni + node.rightOffset].bbox.Contains(point))
				todo[++stackptr] = BVHTraversal(ni + node.rightOffset);
		}
	}

	return result;
}

GLR::BBox GLR::BVH::Get(unsigned index) const
{
	if (!m_flatTree)
		return BVHFlatNode().bbox;

	return m_flatTree[index].bbox;
}

void GLR::BVH::Build(std::vector<BBox>& shapes)
{
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
	std::vector<BVHFlatNode> buildNodes;
	buildNodes.reserve(shapes.size() * 2);

	while(stackptr > 0)
	{
		BVHBuildEntry &bnode(todo[--stackptr]);
		unsigned start = bnode.start;
		unsigned end = bnode.end;
		unsigned nShapes = end - start;

		++m_nNodes;
		node.start = start;
		node.nShapes = nShapes;
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

		buildNodes.push_back(node);

		if(bnode.parent != rootIdentifier)
		{
			--buildNodes[bnode.parent].rightOffset;

			if (buildNodes[bnode.parent].rightOffset == touchedTwice)
				buildNodes[bnode.parent].rightOffset = m_nNodes - 1 - bnode.parent;
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

	m_flatTree = new BVHFlatNode[m_nNodes];
	for (unsigned i = 0; i < m_nNodes; i++)
		m_flatTree[i] = buildNodes[i];
}
