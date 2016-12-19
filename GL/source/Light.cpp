#include "..\include\Light.h"

float CalculateRange(const glm::vec4& color, float exponent, float linear, float constant)
{
	constant -= 256.0f * color.a * glm::max(glm::max(color.x, color.y), color.z);
	return (-linear + sqrt(linear * linear - 4 * exponent * constant)) / (2.0f * exponent);
}


GLR::DirectionalLight::DirectionalLight(const glm::vec3& direction, const glm::vec4& color) : m_direction(glm::normalize(direction)), m_color(color)
{
}

const glm::vec3& GLR::DirectionalLight::GetDirection() const
{
	return m_direction;
}

const glm::vec4& GLR::DirectionalLight::GetColor() const
{
	return m_color;
}

GLR::PointLight::PointLight(const glm::vec3& position, const glm::vec4& color, float exponent, float linear, float constant) : m_position(position), m_range(CalculateRange(color, exponent, linear, constant)), 
m_color(color), m_exponent(exponent), m_linear(linear), m_constant(constant)
{
}

const glm::vec3& GLR::PointLight::GetPosition() const
{
	return m_position;
}

const glm::vec4& GLR::PointLight::GetColor() const
{
	return m_color;
}

float GLR::PointLight::GetRange() const
{
	return m_range;
}

float GLR::PointLight::GetAttenuationExponent() const
{
	return m_exponent;
}

float GLR::PointLight::GetAttenuationLinear() const
{
	return m_linear;
}

float GLR::PointLight::GetAttenuationConstant() const
{
	return m_constant;
}

GLR::SpotLight::SpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec4& color, float exponent, float linear, float constant, float innerConeAngle, float outerConeAngle) : m_position(position),
m_direction(glm::normalize(direction)), m_range(CalculateRange(color, exponent, linear, constant)), m_color(color), m_exponent(exponent), m_linear(linear), m_constant(constant), m_innerCutoff(glm::cos(glm::radians(innerConeAngle))), 
m_outerCutoff(glm::cos(glm::radians(outerConeAngle)))
{
}

const glm::vec3& GLR::SpotLight::GetPosition() const
{
	return m_position;
}

const glm::vec3& GLR::SpotLight::GetDirection() const
{
	return m_direction;
}

const glm::vec4& GLR::SpotLight::GetColor() const
{
	return m_color;
}

float GLR::SpotLight::GetRange() const
{
	return m_range;
}

float GLR::SpotLight::GetAttenuationExponent() const
{
	return m_exponent;
}

float GLR::SpotLight::GetAttenuationLinear() const
{
	return m_linear;
}

float GLR::SpotLight::GetAttenuationConstant() const
{
	return m_constant;
}

float GLR::SpotLight::GetInnerConeAngle() const
{
	return glm::degrees(glm::acos(m_innerCutoff));
}

float GLR::SpotLight::GetOuterConeAngle() const
{
	return glm::degrees(glm::acos(m_outerCutoff));
}