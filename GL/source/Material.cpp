#include <PCH.h>
#include <Material.h>
#include "Texture.h"

GLR::Material::Material(Texture2D* albedoTexture, Texture2D* normalTexture, Texture2D* specularTexture,
	Texture2D* opacityTexture, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular,
	float specularExponent, glm::vec3& emissive)
{
	m_buffer = { 0 };

	if (albedoTexture)
	{
		m_buffer.albedoTexture = albedoTexture->GetHandle();
		if(m_buffer.albedoTexture != 0)
			m_buffer.textureMask |= 1;
	}
	if (normalTexture)
	{
		m_buffer.normalTexture = normalTexture->GetHandle();
		if (m_buffer.normalTexture != 0)
			m_buffer.textureMask |= 1 << 1;
	}
	if (specularTexture)
	{
		m_buffer.specularTexture = specularTexture->GetHandle();
		if (m_buffer.specularTexture != 0)
			m_buffer.textureMask |= 1 << 2;
	}
	if (opacityTexture)
	{
		m_buffer.opacityTexture = opacityTexture->GetHandle();
		if (m_buffer.opacityTexture != 0)
			m_buffer.textureMask |= 1 << 3;
	}
	m_buffer.ambient = ambient;
	m_buffer.diffuse = glm::vec4(diffuse, 1.0f);
	m_buffer.specular = specular;
	m_buffer.specularExponent = specularExponent;
	m_buffer.emissive = glm::vec4(emissive, 1.0f);
}

GLR::Material::~Material()
{
}
