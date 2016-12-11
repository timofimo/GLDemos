#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

namespace GLR
{
	class Mesh
	{
	public:
		Mesh(const float* vertexData, unsigned vertexDataCount, const unsigned* indexData, unsigned indexDataCount, const std::vector<GLenum>& attributeTypes);
		~Mesh();

		GLuint GetVertexArray() const;

	private:
		void GetAttributeTypeInformation(GLenum type, unsigned& count, unsigned& baseType) const;

		GLuint m_vbo = 0;
		GLuint m_ibo = 0;
		GLuint m_vao = 0;
	};

}
