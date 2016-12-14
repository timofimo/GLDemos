#pragma once
#include <ExampleBase/Transform.h>
#include <ExampleBase/Input.h>

class Camera
{
public:
	Camera(float fov, float aspect, float nearZ, float farZ) : m_projectionMatrix(glm::perspective(fov, aspect, nearZ, farZ)){}
	~Camera(){}

	void Update(float deltaTime)
	{
		if (Input::GetKey(GLFW_KEY_KP_8))
			m_transform.Rotate(glm::vec3(1.0f * deltaTime, 0.0f, 0.0f));
		if(Input::GetKey(GLFW_KEY_KP_5))
			m_transform.Rotate(glm::vec3(-1.0f * deltaTime, 0.0f, 0.0f));
		if (Input::GetKey(GLFW_KEY_KP_4))
			m_transform.Rotate(glm::vec3(0.0f, 1.0f * deltaTime, 0.0f));
		if (Input::GetKey(GLFW_KEY_KP_6))
			m_transform.Rotate(glm::vec3(0.0f, -1.0f * deltaTime, 0.0f));

		glm::vec3 forward = m_transform.GetForward();
		glm::vec3 right = m_transform.GetRight();

		if (Input::GetKey(GLFW_KEY_W))
			m_transform.Translate(1.0f * deltaTime * forward);
		if (Input::GetKey(GLFW_KEY_S))
			m_transform.Translate(-1.0f * deltaTime * forward);
		if (Input::GetKey(GLFW_KEY_A))
			m_transform.Translate(-1.0f * deltaTime * right);
		if (Input::GetKey(GLFW_KEY_D))
			m_transform.Translate(1.0f * deltaTime * right);
	}

	glm::mat4 GetViewProjectionMatrix()
	{
		return m_transform.GetViewMatrix() * m_projectionMatrix;
	}

private:
	Transform m_transform;
	glm::mat4 m_projectionMatrix;
};