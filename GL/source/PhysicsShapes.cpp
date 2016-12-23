#include "..\include\PhysicsShapes.h"
#pragma warning(push, 0)
#include <glm/gtx/norm.hpp>
#pragma warning(pop)

GLR::BSphere::BSphere() : m_center(), m_radius(0.0f)
{
}

GLR::BSphere::BSphere(const glm::vec3& center, float radius) : m_center(center), m_radius(radius)
{
}

void GLR::BSphere::ExpandToInclude(const glm::vec3& point)
{
	if (glm::distance2(m_center, point) <= m_radius * m_radius)
		return;

	float distance = glm::distance(m_center, point);
	glm::vec3 dir = (point - m_center) / distance;
	m_center = ((-dir * m_radius + m_center) + point) * 0.5f;
	m_radius = (distance + m_radius) * 0.5f;
}

void GLR::BSphere::ExpandToInclude(const BSphere& sphere)
{
	if (glm::distance2(m_center, sphere.Center()) <= glm::pow(m_radius+ sphere.Radius(), 2.0f))
		return;

	float distance = glm::distance(m_center, sphere.Center());
	glm::vec3 dir = (sphere.Center() - m_center) / distance;
	m_center = ((sphere.Center() + dir * sphere.Radius()) + (m_center - dir * m_radius)) * 0.5f;
	m_radius = (distance + sphere.Radius() + m_radius) * 0.5f;
}

const glm::vec3& GLR::BSphere::Center() const
{
	return m_center;
}

float GLR::BSphere::Radius() const
{
	return m_radius;
}

GLR::BBox::BBox() : m_min(), m_max()
{
}

GLR::BBox::BBox(const glm::vec3& point) : m_min(point), m_max(point)
{
}

GLR::BBox::BBox(const glm::vec3& min, const glm::vec3& max) : m_min(min), m_max(max)
{
}

GLR::BBox::BBox(const glm::vec3& point, float halfSize) : m_min(point - glm::vec3(halfSize)), m_max(point + glm::vec3(halfSize))
{
}

GLR::BBox::BBox(const BSphere& sphere) : BBox(sphere.Center(), sphere.Radius())
{
}

GLR::BBox::~BBox()
{
}

void GLR::BBox::ExpandToInclude(const glm::vec3& point)
{
	m_min = glm::min(m_min, point);
	m_max = glm::max(m_max, point);
}

void GLR::BBox::ExpandToInclude(const BBox& box)
{
	m_min = glm::min(m_min, box.m_min);
	m_max = glm::max(m_max, box.m_max);
}

bool GLR::BBox::Contains(const glm::vec3& point) const
{
	return point.x >= m_min.x && point.x <= m_max.x &&
		point.y >= m_min.y && point.y <= m_max.y &&
		point.z >= m_min.z && point.z <= m_max.z;
}

const glm::vec3& GLR::BBox::Min() const
{
	return m_min;
}

const glm::vec3& GLR::BBox::Max() const
{
	return m_max;
}

glm::vec3 GLR::BBox::Center() const
{
	return (m_min + m_max) * 0.5f;
}

unsigned GLR::BBox::MaxDimension() const
{
	unsigned result = 0;
	if (m_max.y - m_min.y > m_max.x - m_min.y) result = 1;
	if (m_max.z - m_min.z > m_max.y - m_min.y) result = 2;
	return result;
}
