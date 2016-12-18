#pragma once
#include <Window.h>
#include <GLRenderer.h>
#include <Utilities.h>
#include "Input.h"

class ExampleBase
{
public:
	ExampleBase(unsigned width, unsigned height, const char* title, bool borderless) : m_window(width, height, title, borderless)
	{
		m_window.SetInputCallback(&Input::KeyCallback);
		GLR::Initialize();
	}

	virtual ~ExampleBase()
	{
	}

	void StartGameLoop()
	{
		double startTime = glfwGetTime();

		while(!m_window.ShouldClose())
		{
			GLR::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			double deltaTime = glfwGetTime() - startTime;
			startTime += deltaTime;

			Update(float(deltaTime));
			Render();
			Input::Update();

			m_window.SwapBuffers();
		}
	}

	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;

private:
	GLR::GLFWWindow m_window;
};
