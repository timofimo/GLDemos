#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <fstream>
#include <snappy/snappy-c.h>
#include <sstream>
#include <memory>

#ifndef NDEBUG
#define LOG(...) { printf("LOG: "); printf(__VA_ARGS__); }
#define LOGW(...){ printf("WARNING: "); printf(__VA_ARGS__); }
#define LOGE(...){ printf("ERROR: "); printf(__VA_ARGS__); }
#else
#define LOG(...) {}
#define LOGW(...){}
#define LOGE(...){}
#endif

struct Matrix : public glm::mat4
{
	Matrix(){}
#ifdef AI_MATRIX4X4_H_INC
	Matrix(const aiMatrix4x4& ai)
	{
		for (unsigned x = 0; x < 4; x++)
		{
			for (unsigned y = 0; y < 4; y++)
				(*this)[x][y] = ai[y][x];
		}
	}
#endif
};

struct VectorKey
{
	float time;
	glm::vec3 vector;
};

struct QuaternionKey
{
	float time;
	glm::quat quat;
};

enum MeshAttributes
{
	TextureCoordinates = 0x01,
	Normals = 0x02,
	Tangents = 0x04,
	Bones = 0x08,
	Indices = 0x10
};

struct Header
{
	unsigned numMeshes = 0;
	unsigned numMaterials = 0;
	unsigned numSkeletons = 0;
	unsigned numAnimations = 0;
	unsigned numTextures = 0;
};

struct Bone
{
	unsigned parent;
	Matrix transform;
	std::vector<unsigned> children;
};

struct Skeleton
{
	std::map<std::string, unsigned> boneNames;
	std::vector<Bone> bones;
};

struct BoneWeight
{
	unsigned boneIndex = 0;
	float weight = 0.0f;
};

struct BoundingSphere
{
	glm::vec3 center;
	float radius;
};

struct BoundingBox
{
	glm::vec3 min;
	glm::vec3 max;
};

struct Mesh
{
	std::string name = "";
	unsigned attributes = 0;
	unsigned materialIndex = 0;
	BoundingBox bBox;
	BoundingSphere bSphere;
	std::vector<float> vertexPositions;
	std::vector<float> textureCoordinates;
	std::vector<float> vertexNormals;
	std::vector<float> vertexTangents;
	std::vector<BoneWeight> vertexBoneWeights;
	std::vector<unsigned> indices;
};

struct Material
{
	std::string name = "";
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float opacity = 1.0f;
	unsigned diffuseTexture = UINT_MAX;
	unsigned specularTexture = UINT_MAX;
	unsigned shininessTexture = UINT_MAX;
	unsigned normalTexture = UINT_MAX;
};

struct AnimationChannel
{
	std::string boneName;
	std::vector<VectorKey> positionKeys;
	std::vector<QuaternionKey> rotationKeys;
	std::vector<VectorKey> scaleKeys;
};

struct Animation
{
	std::string name;
	float duration;
	float ticksPerSecond;
	std::vector<AnimationChannel> channels;
};

void Import(const char* file, std::vector<Mesh>& meshes, std::vector<Skeleton>& skeletons, std::vector<Animation>& animations, std::vector<Material>& materials, std::vector<std::string>& textures)
{
	// Read in the file
	size_t compressedDataSize;
	size_t uncompressedDataSize;
	std::unique_ptr<char> compressedData;
	std::unique_ptr<char> uncompressedData;

	std::ifstream fileStream(file, std::ios::in | std::ios::binary);
	if (fileStream.is_open())
	{
		fileStream.seekg(0, std::ios::end);
		compressedDataSize = fileStream.tellg();
		fileStream.seekg(0);
		compressedData.reset(new char[compressedDataSize]);
		fileStream.read(compressedData.get(), compressedDataSize);
		fileStream.close();
	}
	else
	{
		LOGE("Failed to open file for import. %s\n", file);
		return;
	}

	// Get the uncompressed data size
	snappy_status status = snappy_uncompressed_length(compressedData.get(), compressedDataSize, &uncompressedDataSize);
	if (status != SNAPPY_OK)
	{
		LOGE("Couldn't get the size of the uncompressed data. %s\n", file);
		return;
	}
	uncompressedData.reset(new char[uncompressedDataSize]);

	// Uncompress the data
	status = snappy_uncompress(compressedData.get(), compressedDataSize, uncompressedData.get(), &uncompressedDataSize);
	if(status != SNAPPY_OK)
	{
		LOGE("Couldn't decompress the data. %s\n", file);
		return;
	}

	// Create a string stream for easy reading
	std::stringstream inputStream;
	inputStream.write(uncompressedData.get(), uncompressedDataSize);
	inputStream.seekg(0, std::ios::beg);

	// Read in the header
	Header header;
	inputStream.read((char*)&header, sizeof(Header));

	char stringBuffer[128];

	// Read in the meshes
	meshes.resize(header.numMeshes);
	for (unsigned i = 0; i < header.numMeshes; i++)
	{
		unsigned uSize;

		// Read the name
		inputStream.read((char*)&uSize, sizeof(unsigned));
		inputStream.read(stringBuffer, uSize);
		meshes[i].name = std::string(stringBuffer, uSize);

		// Read the attributes, material index, bounding box & bounding sphere
		inputStream.read((char*)&meshes[i].attributes, sizeof(unsigned) * 2 + sizeof(BoundingBox) + sizeof(BoundingSphere));

		// Read the vertex positions
		inputStream.read((char*)&uSize, sizeof(unsigned));
		meshes[i].vertexPositions.resize(uSize);
		inputStream.read((char*)meshes[i].vertexPositions.data(), uSize * sizeof(float));

		// Read the texture coordinates if present
		if ((meshes[i].attributes & TextureCoordinates) > 0)
		{
			inputStream.read((char*)&uSize, sizeof(unsigned));
			meshes[i].textureCoordinates.resize(uSize);
			inputStream.read((char*)meshes[i].textureCoordinates.data(), uSize * sizeof(float));
		}

		// Read the vertex normals if present
		if ((meshes[i].attributes & Normals) > 0)
		{
			inputStream.read((char*)&uSize, sizeof(unsigned));
			meshes[i].vertexNormals.resize(uSize);
			inputStream.read((char*)meshes[i].vertexNormals.data(), uSize * sizeof(float));
		}

		// Read the vertex tangents if present
		if ((meshes[i].attributes & Tangents) > 0)
		{
			inputStream.read((char*)&uSize, sizeof(unsigned));
			meshes[i].vertexTangents.resize(uSize);
			inputStream.read((char*)meshes[i].vertexTangents.data(), uSize * sizeof(float));
		}

		// Read the vertex bone weights if present
		if ((meshes[i].attributes & Bones) > 0)
		{
			inputStream.read((char*)&uSize, sizeof(unsigned));
			meshes[i].vertexBoneWeights.resize(uSize);
			inputStream.read((char*)meshes[i].vertexBoneWeights.data(), uSize * sizeof(BoneWeight));
		}

		// Read the indices if present
		if ((meshes[i].attributes & Indices) > 0)
		{
			inputStream.read((char*)&uSize, sizeof(unsigned));
			meshes[i].indices.resize(uSize);
			inputStream.read((char*)meshes[i].indices.data(), uSize * sizeof(unsigned));
		}
	}

	// Read in the skeletons
	skeletons.resize(header.numSkeletons);
	for (unsigned i = 0; i < header.numSkeletons; i++)
	{
		unsigned uSize;

		// Read in the bone map
		unsigned numBones;
		inputStream.read((char*)&numBones, sizeof(unsigned));
		for (unsigned j = 0; j < numBones; j++)
		{
			// Read the name
			inputStream.read((char*)&uSize, sizeof(unsigned));
			inputStream.read(stringBuffer, uSize);
			std::string boneName = std::string(stringBuffer, uSize);

			// Read the index
			inputStream.read((char*)&uSize, sizeof(unsigned));

			// Add bone to map
			skeletons[i].boneNames[boneName] = uSize;
		}

		// Read in the bones
		skeletons[i].bones.resize(numBones);
		for (unsigned j = 0; j < numBones; j++)
		{
			// Read the parent and offset matrix
			inputStream.read((char*)&skeletons[i].bones[j].parent, sizeof(unsigned) + sizeof(Matrix));

			// Read the children
			inputStream.read((char*)&uSize, sizeof(unsigned));
			if (uSize > 0)
			{
				skeletons[i].bones[j].children.resize(uSize);
				inputStream.read((char*)skeletons[i].bones[j].children.data(), sizeof(unsigned) * uSize);
			}
		}
	}

	// Read in the animations
	animations.resize(header.numAnimations);
	for (unsigned i = 0; i < header.numAnimations; i++)
	{
		unsigned uSize;

		// Read the name
		inputStream.read((char*)&uSize, sizeof(unsigned));
		inputStream.read(stringBuffer, uSize);
		animations[i].name = std::string(stringBuffer, uSize);

		// Read the duration
		inputStream.read((char*)&animations[i].duration, sizeof(float));

		// Read the ticks per second
		inputStream.read((char*)&animations[i].ticksPerSecond, sizeof(float));

		// Read the number of animation channels
		unsigned numChannels;
		inputStream.read((char*)&numChannels, sizeof(unsigned));

		// Read the animation channels
		animations[i].channels.resize(numChannels);
		for (unsigned j = 0; j < numChannels; j++)
		{
			// Read the bone name
			inputStream.read((char*)&uSize, sizeof(unsigned));
			inputStream.read(stringBuffer, uSize);
			animations[i].channels[j].boneName = std::string(stringBuffer, uSize);

			// Read the position keys
			inputStream.read((char*)&uSize, sizeof(unsigned));
			animations[i].channels[j].positionKeys.resize(uSize);
			inputStream.read((char*)animations[i].channels[j].positionKeys.data(), uSize * sizeof(VectorKey));

			// Read the rotation keys
			inputStream.read((char*)&uSize, sizeof(unsigned));
			animations[i].channels[j].rotationKeys.resize(uSize);
			inputStream.read((char*)animations[i].channels[j].rotationKeys.data(), uSize * sizeof(QuaternionKey));

			// Read the scale keys
			inputStream.read((char*)&uSize, sizeof(unsigned));
			animations[i].channels[j].scaleKeys.resize(uSize);
			inputStream.read((char*)animations[i].channels[j].scaleKeys.data(), uSize * sizeof(VectorKey));
		}
	}

	// Read the materials
	materials.resize(header.numMaterials);
	for(unsigned i = 0; i < header.numMaterials; i++)
	{
		unsigned uSize;

		// Read the material name
		inputStream.read((char*)&uSize, sizeof(unsigned));
		inputStream.read(stringBuffer, uSize);
		materials[i].name = std::string(stringBuffer, uSize);

		// Read the color values and texture indices
		inputStream.read((char*)&materials[i].diffuseColor, sizeof(glm::vec3) * 2 + sizeof(float) + sizeof(unsigned) * 4);
	}

	// Read the textures
	textures.resize(header.numTextures);
	for(unsigned i = 0; i < header.numTextures; i++)
	{
		unsigned uSize;
		inputStream.read((char*)&uSize, sizeof(unsigned));
		inputStream.read(stringBuffer, uSize);
		textures[i] = std::string(stringBuffer, uSize);
	}
}