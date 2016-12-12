#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "MeshBuffer.h"

namespace GLR
{
	class Mesh
	{
	public:
		Mesh(const std::string& name, const std::shared_ptr<MeshBuffer>& meshBuffer, unsigned offset, unsigned size);
		Mesh(const std::string& name, const float* vertexData, unsigned vertexDataCount, const unsigned* indexData, unsigned indexDataCount, const std::vector<GLenum>& attributeTypes);
		~Mesh();

		GLuint GetVertexArray() const;
		const std::vector<GLenum>& GetAttributes() const;
		std::shared_ptr<MeshBuffer> GetMeshBuffer() const;

	private:
		std::string m_name;
		std::shared_ptr<MeshBuffer> m_meshBuffer;
		unsigned int m_bufferOffset = 0;
		unsigned int m_bufferSize = 0;
	};

}
