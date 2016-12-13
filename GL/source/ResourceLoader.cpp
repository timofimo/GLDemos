#include "..\include\ResourceLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>         
#include <assimp/postprocess.h> 
#include "Utilities.h"
#include "Mesh.h"

void GLR::ResourceLoader::LoadMeshes(const char* file, std::vector<Mesh>& meshes, EBatchType batchType)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file,
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!scene)
		LOG_E(importer.GetErrorString());

	for(unsigned i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];

		unsigned size = sizeof(glm::vec3) + (mesh->HasTextureCoords(0) ? sizeof(glm::vec2) : 0) + (mesh->HasNormals() ? sizeof(glm::vec3) : 0) + (mesh->HasTangentsAndBitangents() ? sizeof(glm::vec3) : 0);
		std::vector<float> interleavedData(size * mesh->mNumVertices);

		for(unsigned j = 0; j < mesh->mNumVertices; j++)
		{
			memcpy(&interleavedData[j * size], &mesh->mVertices[j], sizeof(glm::vec3));
			if (mesh->HasTextureCoords(0))
				memcpy(&interleavedData[j * size + 3], &mesh->mTextureCoords[0][j], sizeof(glm::vec2));
			if (mesh->HasNormals())
				memcpy(&interleavedData[j * size + 5], &mesh->mNormals[j], sizeof(glm::vec3));
			if (mesh->HasTangentsAndBitangents())
				memcpy(&interleavedData[j * size + 8], &mesh->mTangents[j], sizeof(glm::vec3));
		}

		std::vector<unsigned> indexData(mesh->mNumFaces * 3);
		for(unsigned j = 0; j < mesh->mNumFaces; j++)
			memcpy(&indexData[j * 3], mesh->mFaces[j].mIndices, sizeof(unsigned) * 3);

		std::vector<GLenum> attributeTypes;
		attributeTypes.push_back(GL_FLOAT_VEC3);
		if (mesh->HasTextureCoords(0))
			attributeTypes.push_back(GL_FLOAT_VEC2);
		if (mesh->HasNormals())
			attributeTypes.push_back(GL_FLOAT_VEC3);
		if (mesh->HasTangentsAndBitangents())
			attributeTypes.push_back(GL_FLOAT_VEC3);

		if(batchType == EBatchType::None)
			meshes.push_back(Mesh(mesh->mName.C_Str(), interleavedData.data(), mesh->mNumVertices * size, indexData.data(), mesh->mNumFaces * 3, attributeTypes));
		else if(batchType == EBatchType::PerFile)
		{
			if(m_currentMeshBuffer == nullptr)
				m_currentMeshBuffer = std::make_shared<MeshBuffer>(interleavedData.data(), unsigned(interleavedData.size()), indexData.data(), unsigned(indexData.size()), attributeTypes);

			if(m_currentMeshBuffer->GetAttributes() != attributeTypes)
			{
				m_currentMeshBuffer->Finish();
				m_currentMeshBuffer.reset();
				m_currentMeshBuffer = std::make_shared<MeshBuffer>(interleavedData.data(), unsigned(interleavedData.size()), indexData.data(), unsigned(indexData.size()), attributeTypes);
			}

			unsigned offset = m_currentMeshBuffer->GetNumberOfIndices();
			m_currentMeshBuffer->AddVertices(interleavedData.data(), unsigned(interleavedData.size()), indexData.data(), unsigned(indexData.size()), attributeTypes);
			meshes.push_back(Mesh(mesh->mName.C_Str(), m_currentMeshBuffer, offset, unsigned(indexData.size())));

			if (i == scene->mNumMeshes - 1)
			{
				m_currentMeshBuffer->Finish();
				return;
			}
		}
	}
}
