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
