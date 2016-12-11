#include "..\include\Mesh.h"
#include "Utilities.h"

GLR::Mesh::Mesh(const float* vertexData, unsigned vertexDataCount, const unsigned* indexData, unsigned indexDataCount, const std::vector<GLenum>& attributeTypes)
{
	glGenVertexArrays(1, &m_vao);
	GL_GET_ERROR();

	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	GL_GET_ERROR();

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	GL_GET_ERROR();

	glBufferData(GL_ARRAY_BUFFER, vertexDataCount * sizeof(float), vertexData, GL_STATIC_DRAW);
	GL_GET_ERROR();

	struct AttributeInformation
	{
		unsigned size;
		unsigned type;
		UINT64 offset;
	};
	std::vector<AttributeInformation> attributeInformation(attributeTypes.size());
	unsigned stride = 0;
	for (unsigned i = 0; i < unsigned(attributeTypes.size()); i++)
	{
		GetAttributeTypeInformation(attributeTypes[i], attributeInformation[i].size, attributeInformation[i].type);
		attributeInformation[i].offset = stride;
		stride += attributeInformation[i].size * 4;
	}

	for (unsigned i = 0; i < unsigned(attributeTypes.size()); i++)
	{
		glVertexAttribPointer(i, attributeInformation[i].size, attributeInformation[i].type, GL_FALSE, stride, reinterpret_cast<void*>(attributeInformation[i].offset));
		glEnableVertexAttribArray(i);
		GL_GET_ERROR();
	}

	glGenBuffers(1, &m_ibo);
	GL_GET_ERROR();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	GL_GET_ERROR();

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataCount * sizeof(unsigned), indexData, GL_STATIC_DRAW);
	GL_GET_ERROR();

	glBindVertexArray(0);
	GL_GET_ERROR();
}

GLR::Mesh::~Mesh()
{
	if(m_vbo != 0)
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

GLuint GLR::Mesh::GetVertexArray() const
{
	return m_vao;
}

void GLR::Mesh::GetAttributeTypeInformation(GLenum type, unsigned& size, unsigned& baseType) const
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
