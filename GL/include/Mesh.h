#pragma once
#include "MeshBuffer.h"

namespace GLR
{
	class Mesh : public ManagedItem<Mesh>
	{
	public:
		Mesh(const std::string& name, const std::shared_ptr<MeshBuffer>& meshBuffer, unsigned offset, unsigned size);
		Mesh(const std::string& name, const float* vertexData, unsigned vertexDataCount, const unsigned* indexData, unsigned indexDataCount, const std::vector<GLenum>& attributeTypes);
		~Mesh();

		bool operator() (const Mesh& lhs, const Mesh& rhs) const;

		GLuint GetVertexArray() const;
		const std::vector<GLenum>& GetAttributes() const;
		std::shared_ptr<MeshBuffer> GetMeshBuffer() const;
		unsigned GetIndexOffset() const;
		unsigned GetIndexOffsetBytes() const;
		unsigned GetIndexCount() const;

	private:
		std::shared_ptr<MeshBuffer> m_meshBuffer;
		unsigned int m_bufferOffset = 0;
		unsigned int m_bufferSize = 0;
	};

}
