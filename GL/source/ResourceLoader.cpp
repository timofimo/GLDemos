#include <PCH.h>
#include <ResourceLoader.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>         
#include <assimp/postprocess.h> 
#include <Mesh.h>
#include "Material.h"
#include "Texture.h"

std::shared_ptr<GLR::MeshBuffer> GLR::ResourceLoader::m_currentMeshBuffer;

const aiScene* ImportScene(const char* file, Assimp::Importer* importer)
{
	const aiScene* scene = importer->ReadFile(file,
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_FlipUVs);

	if (!scene)
	{
		LOG_W(importer->GetErrorString());
		return nullptr;
	}

	return scene;
}

void GLR::ResourceLoader::LoadScene(const char* file, std::vector<Mesh>& meshes, EBatchType batchType,
	std::vector<Material>& materials, std::vector<unsigned>& materialIndices)
{
	Assimp::Importer importer;
	const aiScene* scene = ImportScene(file, &importer);
	if (!scene)
		return;

	LoadMeshes(scene, meshes, batchType, materialIndices);
	LoadMaterials(scene, file, materials);
}

void GLR::ResourceLoader::LoadMeshes(const char* file, std::vector<Mesh>& meshes, EBatchType batchType)
{
	Assimp::Importer importer;
	const aiScene* scene = ImportScene(file, &importer);
	if (!scene)
		return;

	std::vector<unsigned> materialIndices;

	LoadMeshes(scene, meshes, batchType, materialIndices);
}

void GLR::ResourceLoader::LoadMaterials(const char* file, std::vector<Material>& materials)
{
	Assimp::Importer importer;
	const aiScene* scene = ImportScene(file, &importer);
	if (!scene)
		return;

	LoadMaterials(file, materials);
}

void GLR::ResourceLoader::LoadMeshes(const void* aiscene, std::vector<Mesh>& meshes, EBatchType batchType, std::vector<unsigned>& materialIndices)
{
	const aiScene* scene = static_cast<const aiScene*>(aiscene);

	unsigned materialIndexOffset = materialIndices.size();
	materialIndices.reserve(scene->mNumMeshes);

	for(unsigned i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];

		unsigned size = 3 + (mesh->HasTextureCoords(0) ? 2 : 0) + (mesh->HasNormals() ? 3 : 0) + (mesh->HasTangentsAndBitangents() ? 3 : 0);
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
		unsigned vertexOffset = 0;
		if (m_currentMeshBuffer)
			vertexOffset = m_currentMeshBuffer->GetNumberOfVertices();

		for (unsigned j = 0; j < mesh->mNumFaces; j++)
		{
			if (mesh->mFaces[j].mNumIndices != 3)
				LOG_E("Meshes should be triangulated");
			memcpy(&indexData[j * 3], mesh->mFaces[j].mIndices, sizeof(unsigned) * 3);
			if(batchType == EBatchType::PerFile)
			{
				for (unsigned k = 0; k < 3; k++)
					indexData[j * 3 + k] += vertexOffset;
			}
		}

		std::vector<GLenum> attributeTypes;
		attributeTypes.push_back(GL_FLOAT_VEC3);
		if (mesh->HasTextureCoords(0))
			attributeTypes.push_back(GL_FLOAT_VEC2);
		if (mesh->HasNormals())
			attributeTypes.push_back(GL_FLOAT_VEC3);
		if (mesh->HasTangentsAndBitangents())
			attributeTypes.push_back(GL_FLOAT_VEC3);

		materialIndices.push_back(mesh->mMaterialIndex + materialIndexOffset);

		if(batchType == EBatchType::None)
			meshes.push_back(Mesh(mesh->mName.C_Str(), interleavedData.data(), mesh->mNumVertices * size, indexData.data(), mesh->mNumFaces * 3, attributeTypes));
		else if(batchType == EBatchType::PerFile)
		{
			unsigned offset = 0;

			if (m_currentMeshBuffer == nullptr)
				m_currentMeshBuffer = std::make_shared<MeshBuffer>(interleavedData.data(), unsigned(interleavedData.size()), indexData.data(), unsigned(indexData.size()), attributeTypes);
			else
			{
				offset = m_currentMeshBuffer->GetNumberOfIndices();
				if (m_currentMeshBuffer->GetAttributes() != attributeTypes)
				{
					m_currentMeshBuffer->Finish();
					m_currentMeshBuffer.reset();
					m_currentMeshBuffer = std::make_shared<MeshBuffer>(interleavedData.data(), unsigned(interleavedData.size()), indexData.data(), unsigned(indexData.size()), attributeTypes);
				}

				m_currentMeshBuffer->AddVertices(interleavedData.data(), unsigned(interleavedData.size()), indexData.data(), unsigned(indexData.size()), attributeTypes);
			}
			meshes.push_back(Mesh(mesh->mName.C_Str(), m_currentMeshBuffer, offset, unsigned(indexData.size())));

			if (i == scene->mNumMeshes - 1)
			{
				m_currentMeshBuffer->Finish();
				m_currentMeshBuffer.reset();
				return;
			}
		}
	}
}

void GLR::ResourceLoader::LoadMaterials(const void* aiscene, const char* file, std::vector<Material>& materials)
{
	const aiScene* scene = static_cast<const aiScene*>(aiscene);

	std::string filePath(file);
	size_t pos = filePath.find_last_of('/');
	filePath.erase(pos + 1, UINT_MAX);

	materials.reserve(scene->mNumMaterials);

	for (unsigned i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];

		aiString matName;
		material->Get(AI_MATKEY_NAME, matName);
		const std::string name(matName.C_Str());

		float shininess, shininessStrength;
		material->Get(AI_MATKEY_SHININESS, shininess);
		material->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength);

		aiColor3D ambient, diffuse, specular, emissive, transparent;
		material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
		material->Get(AI_MATKEY_COLOR_TRANSPARENT, transparent);

		Texture2D* albedoTexture = nullptr;
		Texture2D* normalTexture = nullptr;
		Texture2D* specularTexture = nullptr;
		Texture2D* opacityTexture = nullptr;

		aiString texturePath;
		if(material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
		{
			albedoTexture = GLR::ManagedItem<Texture2D>::GetItem(name + "_albedo");
			if(!albedoTexture)
				albedoTexture = new Texture2D(name + "_albedo", filePath + std::string(texturePath.C_Str()), FILTERING_MIP_MAP | FILTERING_LINEAR);
		}
		if (material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS)
		{
			normalTexture = GLR::ManagedItem<Texture2D>::GetItem(name + "_normal");
			if (!normalTexture)
				normalTexture = new Texture2D(name + "_normal", filePath + std::string(texturePath.C_Str()), FILTERING_MIP_MAP | FILTERING_LINEAR);
		}
		if (material->GetTexture(aiTextureType_SPECULAR, 0, &texturePath) == AI_SUCCESS)
		{
			specularTexture = GLR::ManagedItem<Texture2D>::GetItem(name + "_specular");
			if (!specularTexture)
				specularTexture = new Texture2D(name + "_specular", filePath + std::string(texturePath.C_Str()), FILTERING_MIP_MAP | FILTERING_LINEAR);
		}

		if (material->GetTexture(aiTextureType_OPACITY, 0, &texturePath) == AI_SUCCESS)
		{
			opacityTexture = GLR::ManagedItem<Texture2D>::GetItem(name + "_opacity");
			if (!opacityTexture)
				opacityTexture = new Texture2D(name + "_opacity", filePath + std::string(texturePath.C_Str()), FILTERING_MIP_MAP | FILTERING_LINEAR);
		}

		materials.push_back(Material(albedoTexture, normalTexture, specularTexture, opacityTexture, *reinterpret_cast<glm::vec3*>(&ambient), *reinterpret_cast<glm::vec3*>(&diffuse), *reinterpret_cast<glm::vec3*>(&specular), shininess, *reinterpret_cast<glm::vec3*>(&emissive)));
	}
}