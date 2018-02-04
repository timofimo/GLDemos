#pragma once

namespace GLR
{
	class BSphere
	{
	public:
		BSphere();
		BSphere(const glm::vec3& center, float radius);
		
		void ExpandToInclude(const glm::vec3& point);
		void ExpandToInclude(const BSphere& sphere);

		const glm::vec3& Center() const;
		float Radius() const;

	private:
		glm::vec3 m_center;
		float m_radius;
	};

	class BBox
	{
	public:
		BBox();
		BBox(const glm::vec3& point);
		BBox(const glm::vec3& min, const glm::vec3& max);
		BBox(const glm::vec3& point, float halfSize);
		BBox(const BSphere& sphere);
		~BBox();

		void ExpandToInclude(const glm::vec3& point);
		void ExpandToInclude(const BBox& box);
		bool Contains(const glm::vec3& point) const;

		const glm::vec3& Min() const;
		const glm::vec3& Max() const;
		glm::vec3 Center() const;
		unsigned MaxDimension() const;

	private:
		glm::vec3 m_min, m_max;
	};
}
