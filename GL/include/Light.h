#pragma once
#include <glm/glm.hpp>

namespace GLR
{
	class DirectionalLight
	{
	public:
		DirectionalLight(const glm::vec3& direction, const glm::vec4& color);

		const glm::vec3& GetDirection() const;
		const glm::vec4& GetColor() const;

	private:
		glm::vec3 m_direction;
		glm::vec4 m_color;
	};

	class PointLight
	{
	public:
		PointLight(const glm::vec3& position, const glm::vec4& color, float exponent, float linear, float constant);

		const glm::vec3& GetPosition() const;
		const glm::vec4& GetColor() const;
		float GetRange() const;
		float GetAttenuationExponent() const;
		float GetAttenuationLinear() const;
		float GetAttenuationConstant() const;

	private:
		glm::vec3 m_position;
		float m_range;
		glm::vec4 m_color;
		float m_exponent, m_linear, m_constant;
	};

	class SpotLight
	{
	public:
		SpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec4& color, float exponent, float linear, float constant, float innerConeAngle, float outerConeAngle);

		const glm::vec3& GetPosition() const;
		const glm::vec3& GetDirection() const;
		const glm::vec4& GetColor() const;
		float GetRange() const;
		float GetAttenuationExponent() const;
		float GetAttenuationLinear() const;
		float GetAttenuationConstant() const;
		float GetInnerConeAngle() const;
		float GetOuterConeAngle() const;

	private:
		glm::vec3 m_position;
		glm::vec3 m_direction;
		float m_range;
		glm::vec4 m_color;
		float m_exponent, m_linear, m_constant;
		float m_innerCutoff, m_outerCutoff;
	};
}