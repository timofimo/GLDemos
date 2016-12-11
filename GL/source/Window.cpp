#include "..\include\Window.h"
#include <stdexcept>
#include "Utilities.h"
#include <glad/glad.h>

GLR::GLFWWindow::GLFWWindow(): m_window(nullptr), m_width(0), m_height(0)
{
}

GLR::GLFWWindow::GLFWWindow(unsigned width, unsigned height, const char* title, bool borderless)
{
	Initialize(width, height, title, borderless);
}

GLR::GLFWWindow::~GLFWWindow()
{
	Destroy();
}

void GLR::GLFWWindow::Initialize(unsigned width, unsigned height, const char* title, bool borderless)
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

	if(width == unsigned(mode->width) && height == unsigned(mode->height))
		m_window = glfwCreateWindow(width, height, title, monitor, nullptr);	// Fullscreen
	else
	{
		m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);	// Windowed

		// Center the window
		glfwSetWindowPos(m_window, int((mode->width - width) * 0.5f), int((mode->height - height) * 0.5f));
	}

	if (!m_window)
		throw std::runtime_error("Failed to create GLFW window.");

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);

	m_width = width;
	m_height = height;

	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
		LOG_E("Failed to initialize OpenGL context");
}

void GLR::GLFWWindow::Destroy()
{
	glfwTerminate();
}

void GLR::GLFWWindow::GetWindowRes(unsigned& outWidth, unsigned& outHeight)
{
	outWidth = m_width;
	outHeight = m_height;
}

bool GLR::GLFWWindow::ShouldClose()
{
	return glfwWindowShouldClose(m_window) > 0;
}

void GLR::GLFWWindow::SwapBuffers()
{
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}
