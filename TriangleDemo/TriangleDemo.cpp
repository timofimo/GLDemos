#include "GLRenderer.h"
#include "Window.h"
#include "Shader.h"
#include "Mesh.h"

int main()
{
	GLR::GLFWWindow window(640, 480, "TriangleDemo", false);

	const float vertices[] = 
		{ 
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 
			0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 0.0f, 0.0f, 1.0f
		};
	const unsigned indices[] = { 0, 1, 2 };
	GLR::Mesh triangle("Triangle", vertices, 15, indices, 3, { GL_FLOAT_VEC2/*Pos*/, GL_FLOAT_VEC3/*Col*/ });

	GLR::Shader shader("D:/Programming/GLDemos/TriangleDemo/res/shaders/triangleDemo.vert", "D:/Programming/GLDemos/TriangleDemo/res/shaders/triangleDemo.frag");

	GLR::BindMesh(triangle);
	GLR::BindShader(shader);

	GLR::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	while(!window.ShouldClose())
	{
		GLR::Clear(GL_COLOR_BUFFER_BIT);

		GLR::DrawIndexed(3);

		window.SwapBuffers();
	}

	return 0;
}
