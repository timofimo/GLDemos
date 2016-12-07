#include "..\include\Window.h"
#include "GLFW/glfw3.h"
#include <stdexcept>

GL::GLFWWindow::GLFWWindow(): m_window(nullptr), m_width(0), m_height(0)
{
}

GL::GLFWWindow::GLFWWindow(unsigned width, unsigned height, const char* title, bool borderless)
{
	Initialize(width, height, title, borderless);
}

GL::GLFWWindow::~GLFWWindow()
{
	Destroy();
}

void GL::GLFWWindow::Initialize(unsigned width, unsigned height, const char* title, bool borderless)
{
	if (!glfwInit())
		throw std::runtime_error("Failed to initialize GLFW.");

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_DECORATED, !borderless); 

	if(width == mode->width && height == mode->height)
		m_window = glfwCreateWindow(width, height, title, monitor, nullptr);	// Fullscreen
	else
	{
		m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);	// Windowed

		// Center the window
		glfwSetWindowPos(m_window, (mode->width - width) * 0.5f, (mode->height - height) * 0.5f);
	}

	if (!m_window)
		throw std::runtime_error("Failed to create GLFW window.");

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);

	m_width = width;
	m_height = height;
}

void GL::GLFWWindow::Destroy()
{
	glfwTerminate();
}

void GL::GLFWWindow::GetWindowRes(unsigned& outWidth, unsigned& outHeight)
{
	outWidth = m_width;
	outHeight = m_height;
}

bool GL::GLFWWindow::ShouldClose()
{
	return glfwWindowShouldClose(m_window) > 0;
}

void GL::GLFWWindow::SwapBuffers()
{
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}
