#include <PCH.h>
#include <Mesh.h>

GLR::Mesh::Mesh(const std::string& name, const std::shared_ptr<MeshBuffer>& meshBuffer, unsigned offset, unsigned size) : ManagedItem<GLR::Mesh>(name, this)
{
	m_meshBuffer = meshBuffer;
	m_bufferOffset = offset;
	m_bufferSize = size;
}

GLR::Mesh::Mesh(const std::string& name, const float* vertexData, unsigned vertexDataCount, const unsigned* indexData, unsigned indexDataCount, const std::vector<GLenum>& attributeTypes) : ManagedItem<GLR::Mesh>(name, this)
{
	SetBoundingShapesFromVertexData(vertexData, vertexDataCount, attributeTypes);
	m_meshBuffer = std::make_shared<MeshBuffer>(vertexData, vertexDataCount, indexData, indexDataCount, attributeTypes);
	m_meshBuffer->Finish();
	m_bufferOffset = 0;
	m_bufferSize = indexDataCount;
}

GLR::Mesh::~Mesh()
{
}

bool GLR::Mesh::operator()(const Mesh& lhs, const Mesh& rhs) const
{
	return lhs.GetVertexArray() < rhs.GetVertexArray();
}

GLuint GLR::Mesh::GetVertexArray() const
{
	return m_meshBuffer->GetVertexArray();
}

const std::vector<GLenum>& GLR::Mesh::GetAttributes() const
{
	return m_meshBuffer->GetAttributes();
}

std::shared_ptr<GLR::MeshBuffer> GLR::Mesh::GetMeshBuffer() const
{
	return m_meshBuffer;
}

unsigned GLR::Mesh::GetIndexOffset() const
{
	return m_bufferOffset;
}

unsigned GLR::Mesh::GetIndexOffsetBytes() const
{
	return m_bufferOffset * sizeof(unsigned);
}

unsigned GLR::Mesh::GetIndexCount() const
{
	return m_bufferSize;
}

const GLR::BoundingBox& GLR::Mesh::GetBoundingBox() const
{
	return m_boundingBox;
}

const GLR::BoundingSphere & GLR::Mesh::GetBoundingSphere() const
{
	return m_boundingSphere;
}

void GLR::Mesh::SetBoundingShapesFromVertexData(const float* vertexData, unsigned vertexDataCount,
	const std::vector<GLenum>& attributeTypes)
{
	int stride = 0;
	for(GLenum attr : attributeTypes)
	{
		if (attr == GL_FLOAT_VEC3)
			stride += 3;
		else if (attr == GL_FLOAT_VEC2)
			stride += 2;
		else
			LOG_E("Attribute type not supported.");
	}

	glm::vec3& bb_min = m_boundingBox.min;
	glm::vec3& bb_max = m_boundingBox.max;
	glm::vec3& bs_center = m_boundingSphere.center;
	float& bs_radius = m_boundingSphere.radius;

	bb_min = glm::vec3(FLT_MAX);
	bb_max = glm::vec3(-FLT_MAX);

	glm::vec3 a = *reinterpret_cast<const glm::vec3*>(&vertexData[0]); // Initial point
	glm::vec3 b = a; // Point furthest from a

	int numVertices = vertexDataCount / stride;
	for (int i = 0; i < numVertices; ++i)
	{
		const glm::vec3& v = *reinterpret_cast<const glm::vec3*>(&vertexData[i * stride]);
		bb_min = glm::min(bb_min, v);
		bb_max = glm::max(bb_max, v);

		if (glm::distance2(a, b) < glm::distance2(a, v))
		{
			b = v;
		}
	}

	glm::vec3 c = b; // Point furthest from b

	for (int i = 0; i < numVertices; ++i)
	{
		const glm::vec3& v = *reinterpret_cast<const glm::vec3*>(&vertexData[i * stride]);

		if (glm::distance2(b, c) < glm::distance2(b, v))
		{
			c = v;
		}
	}

	bs_center = (b + c) * 0.5f;
	bs_radius = glm::distance(b, c) * 0.5f;
}
