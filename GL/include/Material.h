#pragma once

namespace GLR
{
	class Texture2D;

	struct MaterialBuffer
	{
		uint64_t albedoTexture, normalTexture, specularTexture, opacityTexture;
		glm::vec3 ambient;
		unsigned textureMask;
		glm::vec4 diffuse;
		glm::vec3 specular;
		float specularExponent;
		glm::vec4 emissive;
	};

	class Material
	{
	public:
		Material(Texture2D* albedoTexture, Texture2D* normalTexture, Texture2D* specularTexture, Texture2D* opacityTexture, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float specularExponent, glm::vec3& emissive);
		~Material();

	private:
		MaterialBuffer m_buffer;
	};
}