#pragma once
#include <ExampleBase/Transform.h>
#include <ExampleBase/Input.h>

class Camera : public Transform
{
public:
	Camera(float fov, float aspect, float nearZ, float farZ) : m_projectionMatrix(glm::perspective(fov, aspect, nearZ, farZ)){}
	~Camera(){}

	void Update(float deltaTime)
	{
		if (Input::GetKey(GLFW_KEY_KP_8))
			Rotate(glm::vec3(-2.0f * deltaTime, 0.0f, 0.0f));
		if(Input::GetKey(GLFW_KEY_KP_5))
			Rotate(glm::vec3(2.0f * deltaTime, 0.0f, 0.0f));
		if (Input::GetKey(GLFW_KEY_KP_4))
			Rotate(glm::vec3(0.0f, 2.0f * deltaTime, 0.0f));
		if (Input::GetKey(GLFW_KEY_KP_6))
			Rotate(glm::vec3(0.0f, -2.0f * deltaTime, 0.0f));

		glm::vec3 forward = GetForward();
		glm::vec3 right = GetRight();

		float speed = 1.0f;
		if (Input::GetKey(GLFW_KEY_LEFT_SHIFT))
			speed = 50.0f;

		if (Input::GetKey(GLFW_KEY_W))
			Translate(speed * deltaTime * forward);
		if (Input::GetKey(GLFW_KEY_S))
			Translate(-speed * deltaTime * forward);
		if (Input::GetKey(GLFW_KEY_A))
			Translate(-speed * deltaTime * right);
		if (Input::GetKey(GLFW_KEY_D))
			Translate(speed * deltaTime * right);
	}

	glm::mat4 GetViewProjectionMatrix()
	{
		return m_projectionMatrix * GetViewMatrix();
	}

	glm::mat4 GetProjectionMatrix() const
	{
		return m_projectionMatrix;
	}
private:
	glm::mat4 m_projectionMatrix;
};