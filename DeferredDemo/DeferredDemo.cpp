#include "GLRenderer.h"
#include "Shader.h"
#include "Mesh.h"
#include <ExampleBase/ExampleBase.h>
#include "ResourceLoader.h"
#include "ExampleBase/Camera.h"
#include "Framebuffer.h"
#include "Light.h"

void APIENTRY error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void*)
{
	printf_s("%u - %u - %u - %u - %u - %s\n", source, type, id, severity, length, message);
}

class DeferredDemo : public ExampleBase
{
public:
	DeferredDemo(unsigned width, unsigned height, const char* title, bool borderless)
		: ExampleBase(width, height, title, borderless), m_camera(glm::radians(60.0f), float(width) / float(height), 0.1f, 2000.0f)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(error_callback, nullptr);
		//GLR::ResourceLoader::LoadMeshes("D:/Programming/GLDemos/DeferredDemo/res/meshes/cube.fbx", m_meshes, GLR::EBatchType::PerFile);
		GLR::ResourceLoader::LoadMeshes("D:/Programming/GLDemos/Resources/sponza/sponza.fbx", m_meshes, GLR::EBatchType::PerFile);

		GLR::Shader::AddGlobalUniformBlock("CameraBlock");
		m_shader = std::make_unique<GLR::Shader>("SimpleShader", "D:/Programming/GLDemos/DeferredDemo/res/shaders/deferredDemo.vert", "D:/Programming/GLDemos/DeferredDemo/res/shaders/deferredDemo.frag");

		m_texture = std::make_unique<GLR::Texture2D>("Texture", "D:/Programming/GLDemos/DeferredDemo/res/textures/brick.jpg", GLR::ETextureFlags::FILTERING_LINEAR | GLR::ETextureFlags::FILTERING_MIP_MAP);

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

		std::shared_ptr<char> lightData;
		unsigned size;
		GLR::PointLight::GetBuffer(lightData, size);

		GLR::PointLight* plp = reinterpret_cast<GLR::PointLight*>(&lightData.get()[0]);
		printf_s("%f\n", plp->GetRange());

		for (unsigned j = 0; j < 1; j++)
		{
			std::vector<GLR::DrawElementsIndirectCommand> drawCommands;
			drawCommands.reserve(m_meshes.size());
			GLuint vao = 0;
			for (unsigned i = 0; i < unsigned(m_meshes.size()); i++)
			{
				if (vao != m_meshes[i].GetVertexArray())
				{
					if (!drawCommands.empty())
					{
						m_commandBuffers.push_back(GLR::CreateDrawCommandsBuffer(drawCommands));
						drawCommands.clear();
					}
					GLR::BindMesh(m_meshes[i]);
					vao = m_meshes[i].GetVertexArray();
				}
				drawCommands.push_back({ m_meshes[i].GetIndexCount(), 1, m_meshes[i].GetIndexOffset(), 0, 0 });
				if (i == unsigned(m_meshes.size()) - 1)
					m_commandBuffers.push_back(GLR::CreateDrawCommandsBuffer(drawCommands));
			}
		}
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
		for (unsigned j = 0; j < 60; j++)
		{
			modelMatrix->Set(glm::translate(glm::vec3(j * 64.0f, 0.0f, 0.0f)));
			for(unsigned i = 0; i < unsigned(m_meshes.size()); i++)
			{
				GLR::BindMesh(m_meshes[i]);
				GLR::DrawIndexed(m_meshes[i].GetIndexCount(), m_meshes[i].GetIndexOffset());
			}

			/*for (unsigned i = 0; i < unsigned(m_commandBuffers.size()); i++)
				GLR::DrawIndexedIndirect(m_commandBuffers[i]);*/
		}

		/*GLR::BindFramebuffer(*m_framebuffer);
		GLR::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		modelMatrix->Set(glm::translate(glm::vec3(0.0f, 0.0f, 3.0f)) * glm::rotate(float(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f)));
		GLR::DrawIndexed(36);
		GLR::UnbindFramebuffer();*/
	}

private:
	std::vector<GLR::Mesh> m_meshes;
	std::unique_ptr<GLR::Shader> m_shader;
	std::unique_ptr<GLR::Texture2D> m_texture;
	Camera m_camera;
	std::unique_ptr<GLR::Framebuffer> m_framebuffer;
	std::vector<unsigned> m_commandBuffers;
};

int main()
{
	DeferredDemo DeferredDemo(640, 480, "DeferredDemo", false);
	DeferredDemo.StartGameLoop();

	return 0;
}