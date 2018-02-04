#include <PCH.h>
#include "GLRenderer.h"
#include "Window.h"
#include "Shader.h"
#include "Mesh.h"
#include <ExampleBase/ExampleBase.h>

class TriangleDemo : public ExampleBase
{
public:
	TriangleDemo(unsigned width, unsigned height, const char* title, bool borderless)
		: ExampleBase(width, height, title, borderless)
	{
		const float vertices[] =
		{
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 0.0f, 0.0f, 1.0f
		};
		const unsigned indices[] = { 0, 1, 2 };
		m_triangle = std::make_unique<GLR::Mesh>("Triangle", vertices, 15, indices, 3, std::vector<GLenum>{ GL_FLOAT_VEC2, GL_FLOAT_VEC3 });

		GLR::Shader::AddGlobalUniformBlock("CameraBlock");
		m_shader = std::make_unique<GLR::Shader>("SimpleShader", "D:/Programming/GLDemos/TriangleDemo/res/shaders/triangleDemo.vert", "D:/Programming/GLDemos/TriangleDemo/res/shaders/triangleDemo.frag");

		GLR::BindMesh(*m_triangle);
		GLR::BindShader(*m_shader);

		GLR::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	}
	~TriangleDemo()
	{
	}

	virtual void Update(float deltaTime) override
	{
	}

	virtual void Render() override
	{
		m_shader->GetUniform("Color")->Set(glm::vec3(1.0f));
		GLR::DrawIndexed(3);
	}

private:
	std::unique_ptr<GLR::Mesh> m_triangle;
	std::unique_ptr<GLR::Shader> m_shader;
};

int main()
{
	TriangleDemo triangleDemo(1280, 720, "TriangleDemo", false);
	triangleDemo.StartGameLoop();

	return 0;
}