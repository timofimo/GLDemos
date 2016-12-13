#include "..\include\MeshBuffer.h"
#include "Utilities.h"


GLR::MeshBuffer::MeshBuffer(const float* vertexData, unsigned vertexDataCount, const unsigned* indexData, unsigned indexDataCount, const std::vector<GLenum>& attributeTypes) : m_attributes(attributeTypes)
{
	m_vertexData.resize(vertexDataCount);
	memcpy(m_vertexData.data(), vertexData, vertexDataCount * sizeof(float));

	m_indexData.resize(indexDataCount);
	memcpy(m_indexData.data(), indexData, indexDataCount * sizeof(unsigned));
}

GLR::MeshBuffer::~MeshBuffer()
{
	if (m_vbo != 0)
	{
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}

	if (m_ibo != 0)
	{
		glDeleteBuffers(1, &m_ibo);
		m_ibo = 0;
	}

	if (m_vao != 0)
	{
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}
}

void GLR::MeshBuffer::Finish()
{
	if (m_vertexData.empty() || m_indexData.empty() || m_attributes.empty())
		LOG_E("Mesh buffer doesn't have all the necessary data");

	if (m_vao != 0)	// Has already been initialised
		return;

	glGenVertexArrays(1, &m_vao);
	GL_GET_ERROR();

	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	GL_GET_ERROR();

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	GL_GET_ERROR();

	glBufferData(GL_ARRAY_BUFFER, m_vertexData.size() * sizeof(float), m_vertexData.data(), GL_STATIC_DRAW);
	GL_GET_ERROR();

	struct AttributeInformation
	{
		unsigned size;
		unsigned type;
		UINT64 offset;
	};
	std::vector<AttributeInformation> attributeInformation(m_attributes.size());
	unsigned stride = 0;
	for (unsigned i = 0; i < unsigned(m_attributes.size()); i++)
	{
		GetAttributeTypeInformation(m_attributes[i], attributeInformation[i].size, attributeInformation[i].type);
		attributeInformation[i].offset = stride;
		stride += attributeInformation[i].size * 4;
	}

	for (unsigned i = 0; i < unsigned(m_attributes.size()); i++)
	{
		glVertexAttribPointer(i, attributeInformation[i].size, attributeInformation[i].type, GL_FALSE, stride, reinterpret_cast<void*>(attributeInformation[i].offset));
		glEnableVertexAttribArray(i);
		GL_GET_ERROR();
	}

	glGenBuffers(1, &m_ibo);
	GL_GET_ERROR();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	GL_GET_ERROR();

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexData.size() * sizeof(unsigned), m_indexData.data(), GL_STATIC_DRAW);
	GL_GET_ERROR();

	glBindVertexArray(0);
	GL_GET_ERROR();
}

void GLR::MeshBuffer::AddVertices(const float* vertexData, unsigned vertexDataCount, const unsigned* indexData, unsigned indexDataCount, const std::vector<GLenum>& attributeTypes)
{
	if (m_vao != 0)
		LOG_E("Trying to add vertex data to finished mesh buffer");

	if (attributeTypes != m_attributes)
		LOG_E("Trying to add vertices of a different attribute configuration to a mesh buffer");

	size_t oldSize = m_vertexData.size();
	m_vertexData.resize(m_vertexData.size() + vertexDataCount);
	memcpy(&m_vertexData[oldSize], vertexData, vertexDataCount * sizeof(float));

	oldSize = m_indexData.size();
	m_indexData.resize(m_indexData.size() + indexDataCount);
	memcpy(&m_indexData[oldSize], indexData, indexDataCount * sizeof(unsigned));
}

GLuint GLR::MeshBuffer::GetVertexArray() const
{
	return m_vao;
}

const std::vector<GLenum>& GLR::MeshBuffer::GetAttributes() const
{
	return m_attributes;
}

unsigned GLR::MeshBuffer::GetNumberOfIndices() const
{
	return unsigned(m_indexData.size());
}

void GLR::MeshBuffer::GetAttributeTypeInformation(GLenum type, unsigned& size, unsigned& baseType) const
{
	switch (type)
	{
	case GL_FLOAT:
		size = 1;
		baseType = GL_FLOAT;
		break;
	case GL_FLOAT_VEC2:
		size = 2;
		baseType = GL_FLOAT;
		break;
	case GL_FLOAT_VEC3:
		size = 3;
		baseType = GL_FLOAT;
		break;
	case GL_FLOAT_VEC4:
		size = 4;
		baseType = GL_FLOAT;
		break;
	case GL_INT:
		size = 1;
		baseType = GL_INT;
		break;
	case GL_INT_VEC2:
		size = 2;
		baseType = GL_INT;
		break;
	case GL_INT_VEC3:
		size = 3;
		baseType = GL_INT;
		break;
	case GL_INT_VEC4:
		size = 4;
		baseType = GL_INT;
		break;
	case GL_UNSIGNED_INT:
		size = 1;
		baseType = GL_UNSIGNED_INT;
		break;
	case GL_UNSIGNED_INT_VEC2:
		size = 2;
		baseType = GL_UNSIGNED_INT;
		break;
	case GL_UNSIGNED_INT_VEC3:
		size = 3;
		baseType = GL_UNSIGNED_INT;
		break;
	case GL_UNSIGNED_INT_VEC4:
		size = 4;
		baseType = GL_INT;
		break;
	case GL_BOOL:
		size = 1;
		baseType = GL_BOOL;
		break;
	case GL_BOOL_VEC2:
		size = 2;
		baseType = GL_BOOL;
		break;
	case GL_BOOL_VEC3:
		size = 3;
		baseType = GL_BOOL;
		break;
	case GL_BOOL_VEC4:
		size = 4;
		baseType = GL_BOOL;
		break;
	default:
		LOG_E("Attribute type not implemented");
	}
}
