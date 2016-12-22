#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <memory>

namespace GLR
{
	template <class T>
	class Light
	{
	public:
		static void Add(T* light)
		{
			m_lights.push_back(light);
		}

		static void Remove(T* light)
		{
			m_lights.erase(std::find(m_lights.begin(), m_lights.end(), light));
		}

		static void GetBuffer(std::shared_ptr<char>& lightData, unsigned& size)
		{
			size = sizeof(T) + (16 - (sizeof(T) % 16));
			lightData.reset(new char[size * m_lights.size()]);
			for(unsigned i = 0; i < unsigned(m_lights.size()); i++)
				memcpy(&lightData.get()[i * size], m_lights[i], sizeof(T));
			size *= unsigned(m_lights.size());
		}

		static unsigned GetCount()
		{
			return unsigned(m_lights.size());
		}

	private:
		static std::vector<T*> m_lights;
	};

	template<class T>
	std::vector<T*> Light<T>::m_lights;

	class DirectionalLight
	{
	public:
		DirectionalLight();
		DirectionalLight(const glm::vec3& direction, const glm::vec4& color);
		~DirectionalLight();

		const glm::vec3& GetDirection() const;
		const glm::vec4& GetColor() const;

	private:
		glm::vec3 m_direction;
		glm::vec4 m_color;
	};

	class PointLight
	{
	public:
		PointLight();
		PointLight(const glm::vec3& position, const glm::vec4& color, float exponent, float linear, float constant);
		~PointLight();

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
		SpotLight();
		SpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec4& color, float exponent, float linear, float constant, float innerConeAngle, float outerConeAngle);
		~SpotLight();

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
