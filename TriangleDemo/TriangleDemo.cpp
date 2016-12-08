#include "GL.h"
#include "Window.h"
#include "glm/glm.hpp"

int main()
{
	GL::GLFWWindow window(640, 480, "TriangleDemo", false);

	const float vertices[] = 
		{ 
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 
			0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 0.0f, 0.0f, 1.0f
		};
	const unsigned indices[] = { 0, 1, 2 };
	Mesh triangle = GL::CreateMesh(vertices, 15, indices, 3, { GL::EAttributeLayout::eVec2/*Pos*/, GL::EAttributeLayout::eVec3/*Col*/ }, 2);

	Shader shader = GL::CreateShader("res/shaders/triangleDemo.vert", "res/shaders/triangleDemo.frag");

	GL::BindMesh(triangle);
	GL::BindShader(shader);

	while(!window.ShouldClose())
	{
		GL::Clear(0.0f, 0.0f, 0.0f, 1.0f);

		GL::DrawIndexed(3);

		window.SwapBuffers();
	}

	return 0;
}
