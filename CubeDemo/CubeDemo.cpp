#include "GLRenderer.h"
#include "Window.h"
#include "Shader.h"
#include "Mesh.h"
#include <ExampleBase/ExampleBase.h>
#include "ResourceLoader.h"
#include "ExampleBase/Camera.h"

class CubeDemo : public ExampleBase
{
public:
	CubeDemo(unsigned width, unsigned height, const char* title, bool borderless)
		: ExampleBase(width, height, title, borderless), m_camera(glm::radians(60.0f), float(width) / float(height), 0.1f, 100.0f)
	{
		GLR::ResourceLoader::LoadMeshes("D:/Programming/GLDemos/CubeDemo/res/meshes/cube.fbx", m_meshes, GLR::EBatchType::PerFile);
		m_cube = &m_meshes[0];

		GLR::Shader::AddGlobalUniformBlock("CameraBlock");
		m_shader = std::make_unique<GLR::Shader>("SimpleShader", "D:/Programming/GLDemos/CubeDemo/res/shaders/CubeDemo.vert", "D:/Programming/GLDemos/CubeDemo/res/shaders/CubeDemo.frag");

		m_texture = std::make_unique<GLR::Texture2D>("Texture", "D:/Programming/GLDemos/CubeDemo/res/textures/brick.jpg", GLR::ETextureFlags::FILTERING_LINEAR | GLR::ETextureFlags::FILTERING_MIP_MAP);

		GLR::BindMesh(*m_cube);
		GLR::BindShader(*m_shader);

		GLR::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		GLR::SetViewport(0, 0, width, height);
		GLR::SetDepthState(true, true, GL_LESS);
		GLR::SetRasterizationState(true, GL_BACK, GL_CCW);

		m_shader->GetUniform("textureSampler")->Set(*m_texture);

		m_camera.SetPosition(glm::vec3(0.0f, 0.0f, -10.0f));
	}
	~CubeDemo()
	{
	}

	virtual void Update(float deltaTime) override
	{
		m_camera.Update(deltaTime);
	}

	virtual void Render() override
	{
		glm::mat4 viewProjectionMatrix = m_camera.GetViewProjectionMatrix();
		m_shader->GetUniformBlock("CameraBlock")->UpdateContents(reinterpret_cast<char*>(&viewProjectionMatrix), sizeof(glm::mat4), 0);

		const GLR::InputParameter* modelMatrix = m_shader->GetUniform("modelMatrix");
		modelMatrix->Set(glm::rotate(float(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f)));
		GLR::DrawIndexed(36);
		modelMatrix->Set(glm::translate(glm::vec3(0.0f, 0.0f, 3.0f)) * glm::rotate(float(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f)));
		GLR::DrawIndexed(36);
	}

private:
	std::vector<GLR::Mesh> m_meshes;
	GLR::Mesh* m_cube;
	std::unique_ptr<GLR::Shader> m_shader;
	std::unique_ptr<GLR::Texture2D> m_texture;
	Camera m_camera;
};

int main()
{
	CubeDemo CubeDemo(640, 480, "CubeDemo", false);
	CubeDemo.StartGameLoop();

	return 0;
}