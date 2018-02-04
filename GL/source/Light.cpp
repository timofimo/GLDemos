#include <PCH.h>
#include <Light.h>


GLR::DirectionalLight::DirectionalLight() : DirectionalLight(glm::vec3(), glm::vec4())
{
}

GLR::DirectionalLight::DirectionalLight(const glm::vec3& direction, const glm::vec4& color) : m_direction(glm::normalize(direction)), m_color(color)
{
	Light<GLR::DirectionalLight>::Add(this);
}

GLR::DirectionalLight::~DirectionalLight()
{
	Light<GLR::DirectionalLight>::Remove(this);
}

const glm::vec3& GLR::DirectionalLight::GetDirection() const
{
	return m_direction;
}

const glm::vec4& GLR::DirectionalLight::GetColor() const
{
	return m_color;
}

GLR::PointLight::PointLight() : PointLight(glm::vec3(), glm::vec4(), 1.0f)
{
}

GLR::PointLight::PointLight(const glm::vec3& position, const glm::vec4& color, float range) : m_position(position), m_range(range), m_color(color)
{
	Light<GLR::PointLight>::Add(this);
}

GLR::PointLight::~PointLight()
{
	Light<GLR::PointLight>::Remove(this);
}

void GLR::PointLight::SetPosition(const glm::vec3& position)
{
	m_position = position;
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

GLR::SpotLight::SpotLight() : SpotLight(glm::vec3(), glm::vec3(), glm::vec4(), 1.0f, 0.0f, 0.0f)
{
}

GLR::SpotLight::SpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec4& color, float range, float innerConeAngle, float outerConeAngle) : m_position(position),
m_direction(glm::normalize(direction)), m_range(range), m_color(color), m_innerCutoff(glm::cos(glm::radians(innerConeAngle))), m_outerCutoff(glm::cos(glm::radians(outerConeAngle)))
{
	Light<GLR::SpotLight>::Add(this);
}

GLR::SpotLight::~SpotLight()
{
	Light<GLR::SpotLight>::Remove(this);
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

float GLR::SpotLight::GetInnerConeAngle() const
{
	return glm::degrees(glm::acos(m_innerCutoff));
}

float GLR::SpotLight::GetOuterConeAngle() const
{
	return glm::degrees(glm::acos(m_outerCutoff));
}
