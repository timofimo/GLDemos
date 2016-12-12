#include "..\include\Mesh.h"
#include "Utilities.h"

GLR::Mesh::Mesh(const std::string& name, const std::shared_ptr<MeshBuffer>& meshBuffer, unsigned offset, unsigned size)
{
	m_name = name;
	m_meshBuffer = meshBuffer;
	m_bufferOffset = offset;
	m_bufferSize = size;
}

GLR::Mesh::Mesh(const std::string& name, const float* vertexData, unsigned vertexDataCount, const unsigned* indexData, unsigned indexDataCount, const std::vector<GLenum>& attributeTypes)
{
	m_name = name;
	m_meshBuffer = std::make_shared<MeshBuffer>(vertexData, vertexDataCount, indexData, indexDataCount, attributeTypes);
	m_bufferOffset = 0;
	m_bufferSize = indexDataCount;
}

GLR::Mesh::~Mesh()
{
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
