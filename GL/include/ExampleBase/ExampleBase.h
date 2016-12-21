#pragma once
#include <Window.h>
#include <GLRenderer.h>
#include <Utilities.h>
#include "Input.h"

#ifdef _DEBUG
void APIENTRY error_callback(GLenum source, GLenum type, GLuint, GLenum severity, GLsizei, const GLchar* message, const void*)
{
	char* src;
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		src = "API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		src = "Window System";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		src = "Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		src = "Third Party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		src = "Application";
		break;
	default:
		src = "Unknown Source";
		break;
	}

	char* typ;
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		typ = "Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		typ = "Deprecated Behaviour";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		typ = "Undefined Behaviour";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		typ = "Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		typ = "Performance";
		break;
	case GL_DEBUG_TYPE_MARKER:
		typ = "Marker";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		typ = "Push Group";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		typ = "Pop Group";
		break;
	default:
		typ = "Unknown Type";
		break;
	}

	char* sev;
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		sev = "High Severity";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		sev = "Medium Severity";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		sev = "Low Severity";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		sev = "Notification";
		return;	// Don't print notifications
	default:
		sev = "Unknown Severity";
		break;
	}
	printf_s("%s::%s::%s - %s\n", src, typ, sev, message);
}
#endif

class ExampleBase
{
public:
	ExampleBase(unsigned width, unsigned height, const char* title, bool borderless) : m_window(width, height, title, borderless)
	{
		m_window.SetInputCallback(&Input::KeyCallback);
		GLR::Initialize();
#ifdef _DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(error_callback, nullptr);
#endif
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
