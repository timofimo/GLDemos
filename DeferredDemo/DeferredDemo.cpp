#include "GLRenderer.h"
#include "Shader.h"
#include "Mesh.h"
#include <ExampleBase/ExampleBase.h>
#include "ResourceLoader.h"
#include "ExampleBase/Camera.h"
#include "Framebuffer.h"
#include "Light.h"

class DeferredDemo : public ExampleBase
{
public:
	DeferredDemo(unsigned width, unsigned height, const char* title, bool borderless)
		: ExampleBase(width, height, title, borderless), m_camera(glm::radians(60.0f), float(width) / float(height), 0.1f, 100.0f)
	{
		GLR::ResourceLoader::LoadMeshes("D:/Programming/GLDemos/DeferredDemo/res/meshes/cube.fbx", m_meshes, GLR::EBatchType::PerFile);
		m_cube = &m_meshes[0];

		GLR::Shader::AddGlobalUniformBlock("CameraBlock");
		m_shader = std::make_unique<GLR::Shader>("SimpleShader", "D:/Programming/GLDemos/DeferredDemo/res/shaders/deferredDemo.vert", "D:/Programming/GLDemos/DeferredDemo/res/shaders/deferredDemo.frag");

		m_texture = std::make_unique<GLR::Texture2D>("Texture", "D:/Programming/GLDemos/DeferredDemo/res/textures/brick.jpg", GLR::ETextureFlags::FILTERING_LINEAR | GLR::ETextureFlags::FILTERING_MIP_MAP);

		GLR::BindMesh(*m_cube);
		GLR::BindShader(*m_shader);

		GLR::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		GLR::SetViewport(0, 0, width, height);
		GLR::SetDepthState(true, true, GL_LESS);
		GLR::SetRasterizationState(true, GL_BACK, GL_CCW);

		m_shader->GetUniform("textureSampler")->Set(*m_texture);

		m_camera.SetPosition(glm::vec3(0.0f, 0.0f, -10.0f));

		m_framebuffer = std::make_unique<GLR::Framebuffer>("TestFBO", width, height, std::vector<GLR::ColorAttachmentDescription>{ {4, GL_UNSIGNED_BYTE} }, GL_DEPTH_COMPONENT32F);

		GLR::PointLight pl(glm::vec3(), glm::vec4(1.0f), 0.01f, 0.0f, 0.0f);
		printf_s("%f\n", pl.GetRange());
	}
	~DeferredDemo()
	{
	}

	virtual void Update(float deltaTime) override
	{
		m_camera.Update(deltaTime);
	}

	virtual void Render() override
	{
		GLR::BindTexture(*m_texture, 0);
		glm::mat4 viewProjectionMatrix = m_camera.GetViewProjectionMatrix();
		m_shader->GetUniformBlock("CameraBlock")->UpdateContents(reinterpret_cast<char*>(&viewProjectionMatrix), sizeof(glm::mat4), 0);

		const GLR::InputParameter* modelMatrix = m_shader->GetUniform("modelMatrix");
		modelMatrix->Set(glm::rotate(float(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f)));
		GLR::DrawIndexed(36);

		GLR::BindFramebuffer(*m_framebuffer);
		GLR::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		modelMatrix->Set(glm::translate(glm::vec3(0.0f, 0.0f, 3.0f)) * glm::rotate(float(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f)));
		GLR::DrawIndexed(36);
		GLR::UnbindFramebuffer();
	}

private:
	std::vector<GLR::Mesh> m_meshes;
	GLR::Mesh* m_cube;
	std::unique_ptr<GLR::Shader> m_shader;
	std::unique_ptr<GLR::Texture2D> m_texture;
	Camera m_camera;
	std::unique_ptr<GLR::Framebuffer> m_framebuffer;
};

int main()
{
	DeferredDemo DeferredDemo(640, 480, "DeferredDemo", false);
	DeferredDemo.StartGameLoop();

	return 0;
}