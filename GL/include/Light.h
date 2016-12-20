#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>

namespace GLR
{
	template <class T>
	class Light
	{
	public:
		Light(T* light)
		{
			m_lights.push_back(light);
		}

		virtual ~Light()
		{
			std::erase(std::find(m_lights.begin(), m_lights.end(), m_this));
		}

		static void GetBuffer(std::shared_ptr<T>& lightData, unsigned& size)
		{
			size = sizeof(T) * unsigned(m_lights.size());
			lightData.reset(new T[m_lights.size()]);
			for(unsigned i = 0; i < unsigned(m_lights.size()); i++)
				memcpy(lightData.get()[i], m_lights[i], sizeof(T));
		}

	private:
		static std::vector<T*> m_lights;
		T* m_this;
	};
	class DirectionalLight : public Light<DirectionalLight>
	{
	public:
		DirectionalLight(const glm::vec3& direction, const glm::vec4& color);

		const glm::vec3& GetDirection() const;
		const glm::vec4& GetColor() const;

	private:
		glm::vec3 m_direction;
		glm::vec4 m_color;
	};

	class PointLight : public Light<PointLight>
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

	class SpotLight : public Light<SpotLight>
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
