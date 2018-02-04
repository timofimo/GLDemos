#pragma once

namespace GLR
{
	class MeshBuffer
	{
	public:

		MeshBuffer(const float* vertexData, unsigned vertexDataCount, const unsigned* indexData, unsigned indexDataCount, const std::vector<GLenum>& attributeTypes);
		~MeshBuffer();

		void Finish();
		void AddVertices(const float* vertexData, unsigned vertexDataCount, const unsigned* indexData, unsigned indexDataCount, const std::vector<GLenum>& attributeTypes);

		GLuint GetVertexArray() const;
		const std::vector<GLenum>& GetAttributes() const;
		unsigned GetNumberOfIndices() const;
		unsigned GetNumberOfVertices() const;
	private:
		void SetAttributes(const std::vector<GLenum>& attributes);
		void GetAttributeTypeInformation(GLenum type, unsigned& size, unsigned& baseType) const;

		std::vector<float> m_vertexData;
		std::vector<unsigned> m_indexData;

		GLuint m_vbo = 0;
		GLuint m_ibo = 0;
		GLuint m_vao = 0;
		std::vector<GLenum> m_attributes;
		unsigned m_attributesSize;
	};

}