#include <PCH.h>
#include "GLRenderer.h"
#include "Shader.h"
#include "Mesh.h"
#include <ExampleBase/ExampleBase.h>
#include "ResourceLoader.h"
#include "ExampleBase/Camera.h"
#include "Framebuffer.h"
#include "Light.h"
#include <glm/gtc/random.hpp>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define TILED_CULLING
//#define DEBUG_OVERDRAW

#define TILE_GROUP_DIM 16
#define TILE_GROUP_SIZE_X SCREEN_WIDTH / TILE_GROUP_DIM
#define TILE_GROUP_SIZE_Y SCREEN_HEIGHT / TILE_GROUP_DIM
#define MAX_LIGHTS_PER_TILE 128

// Quad
const float vertexData[] = 
{
	-1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
};

const unsigned indexData[] =
{
	0, 1, 2,
	0, 2, 3,
};

class TiledDeferredDemo : public ExampleBase
{
public:
	TiledDeferredDemo(unsigned width, unsigned height, const char* title, bool borderless)
		: ExampleBase(width, height, title, borderless), m_camera(glm::radians(60.0f), float(width) / float(height), 0.1f, 2000.0f)
	{
		m_meshes.push_back(GLR::Mesh("Quad", vertexData, 12, indexData, 6, std::vector<GLenum>{ GL_FLOAT_VEC3}));
		GLR::ResourceLoader::LoadMeshes("D:/Programming/GLDemos/Resources/primitives.fbx", m_meshes, GLR::EBatchType::PerFile);
		GLR::ResourceLoader::LoadMeshes("D:/Programming/GLDemos/Resources/sponza/sponza.fbx", m_meshes, GLR::EBatchType::PerFile);

		GLR::Shader::AddGlobalUniformBlock("CameraBlock"); 
		GLR::Shader::AddGlobalUniformBlock("PointLightBlock");
		GLR::Shader::AddGlobalShaderStorageBlock("LightListIndices");
		m_shaders[1] = std::make_unique<GLR::Shader>("geometryShader", "D:/Programming/GLDemos/TiledDeferredDemo/res/shaders/geometryPass.vert", "D:/Programming/GLDemos/TiledDeferredDemo/res/shaders/geometryPass.frag");
#ifndef TILED_CULLING
		m_shaders[2] = std::make_unique<GLR::Shader>("lightShader", "D:/Programming/GLDemos/TiledDeferredDemo/res/shaders/lightPassAdvanced.vert", "D:/Programming/GLDemos/TiledDeferredDemo/res/shaders/lightPassAdvanced.frag");
#else
		m_shaders[0] = std::make_unique<GLR::Shader>("TiledComputeShader", "D:/Programming/GLDemos/TiledDeferredDemo/res/shaders/tiledPass.comp");
		m_shaders[2] = std::make_unique<GLR::Shader>("lightShader", "D:/Programming/GLDemos/TiledDeferredDemo/res/shaders/lightPass.vert", "D:/Programming/GLDemos/TiledDeferredDemo/res/shaders/lightPass.frag");

		m_shaders[0]->GetShaderStorageBlock("LightListIndices")->Reserve(TILE_GROUP_SIZE_X * TILE_GROUP_SIZE_Y * MAX_LIGHTS_PER_TILE * sizeof(int));
#endif

		GLR::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		GLR::SetViewport(0, 0, width, height);
		GLR::SetDepthState(true, true, GL_LESS);
		GLR::SetRasterizationState(true, GL_BACK, GL_CCW);

		m_camera.SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));

		m_framebuffer = std::make_unique<GLR::Framebuffer>("TestFBO", width, height, 0, std::vector<GLR::ColorAttachmentDescription>{ {3, GL_UNSIGNED_BYTE}, {3, GL_UNSIGNED_BYTE} }, GL_DEPTH_COMPONENT32F);

		m_pointLights.reserve(1024);
		for (unsigned i = 0; i < 1024; i++)
			m_pointLights.emplace_back(glm::linearRand(glm::vec3(-150.0f, 0.0f, -80.0f), glm::vec3(450.0f, 140.0f, 240.0f)), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 30.0f);

		std::vector<GLR::DrawElementsIndirectCommand> drawCommands;

		drawCommands.reserve(m_pointLights.size());
		for (unsigned i = 0; i < unsigned(m_pointLights.size()); i++)
			drawCommands.push_back({ m_meshes[2].GetIndexCount(), 1, m_meshes[2].GetIndexOffset(), 0, 0 });
		m_commandBuffers.push_back(GLR::CreateDrawCommandBuffer(drawCommands));

		{
			drawCommands.clear();
			drawCommands.reserve(m_meshes.size());
			GLuint vao = 0;
			for (unsigned i = 0; i < unsigned(m_meshes.size()); i++)
			{
				if (vao != m_meshes[i].GetVertexArray())
				{
					if (!drawCommands.empty())
					{
						m_commandBuffers.push_back(GLR::CreateDrawCommandBuffer(drawCommands));
						drawCommands.clear();
					}
					GLR::BindMesh(m_meshes[i]);
					vao = m_meshes[i].GetVertexArray();
				}
				drawCommands.push_back({ m_meshes[i].GetIndexCount(), 1, m_meshes[i].GetIndexOffset(), 0, 0 });
				if (i == unsigned(m_meshes.size()) - 1)
					m_commandBuffers.push_back(GLR::CreateDrawCommandBuffer(drawCommands));
			}
		}
	}
	~TiledDeferredDemo()
	{
	}

	virtual void Update(float deltaTime) override
	{
		m_camera.Update(deltaTime);
	}

	virtual void Render() override
	{
		// Geometry pass
		GLR::BindShader(*m_shaders[1]);
		GLR::BindFramebuffer(*m_framebuffer);

		struct CameraBlock
		{
			glm::mat4 viewProjectionMatrix;
			glm::mat4 invViewProjectionMatrix;
			glm::mat4 viewMatrix;
			glm::mat4 projectionMatrix;
			glm::vec3 position;
			float padding;
			glm::vec2 screenDimensions;
			glm::vec2 padding2;
		} cameraBlock;
		cameraBlock.viewProjectionMatrix = m_camera.GetViewProjectionMatrix();
		cameraBlock.invViewProjectionMatrix = glm::inverse(cameraBlock.viewProjectionMatrix);
		cameraBlock.viewMatrix = m_camera.GetViewMatrix();
		cameraBlock.projectionMatrix = m_camera.GetProjectionMatrix();
		cameraBlock.position = m_camera.GetWorldPosition();
		cameraBlock.screenDimensions = glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
		m_shaders[1]->GetUniformBlock("CameraBlock")->UpdateContents(reinterpret_cast<char*>(&cameraBlock), sizeof(CameraBlock), 0);

		GLR::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		const GLR::InputParameter* modelMatrix = m_shaders[1]->GetUniform("modelMatrix");
		const GLR::InputParameter* meshColor = m_shaders[1]->GetUniform("meshColor");
		meshColor->Set(glm::vec3(1.0f, 0.9f, 0.9f));

		for (unsigned x = 0; x < 2; x++)
		{
			for (unsigned y = 0; y < 2; y++)
			{
				modelMatrix->Set(glm::scale(glm::vec3(10.0f)) * glm::translate(glm::vec3(x * 32.0f, 0.0f, y * 16.0f)));

				GLR::BindMesh(m_meshes[3]);
				GLR::DrawIndexedIndirect(m_commandBuffers[3]);
			}
		}
		GLR::UnbindShader();

		std::shared_ptr<char> pointLightBuffer;
		unsigned pointLightBufferSize;
		GLR::Light<GLR::PointLight>::GetBuffer(pointLightBuffer, pointLightBufferSize);

#ifdef TILED_CULLING
		// Tiled light culling pass
		GLR::BindShader(*m_shaders[0]);
		m_shaders[0]->GetUniform("depthSampler")->Set(*GLR::Texture2D::GetItem("TestFBO_depth"));
		m_shaders[0]->GetUniform("lightCount")->Set(unsigned(m_pointLights.size()));
		m_shaders[0]->GetUniformBlock("PointLightBlock")->UpdateContents(pointLightBuffer.get(), pointLightBufferSize, 0);
		glDispatchCompute(TILE_GROUP_SIZE_X, TILE_GROUP_SIZE_Y, 1);
#endif
		// Light pass
		GLR::UnbindFramebuffer();
		GLR::SetBlendState(true, GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
		GLR::SetDepthState(false, true, GL_LESS);
		GLR::BindShader(*m_shaders[2]);

#ifndef DEBUG_OVERDRAW
		m_shaders[2]->GetUniform("colorSampler")->Set(*GLR::Texture2D::GetItem("TestFBO_0"));
		m_shaders[2]->GetUniform("normalSampler")->Set(*GLR::Texture2D::GetItem("TestFBO_1"));
		m_shaders[2]->GetUniform("depthSampler")->Set(*GLR::Texture2D::GetItem("TestFBO_depth"));		
#endif

#ifdef TILED_CULLING
		GLR::BindMesh(m_meshes[0]);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		GLR::DrawIndexed(6);
		GLR::UnbindShader();
#else
		GLR::SetRasterizationState(true, GL_FRONT, GL_CCW);
		GLR::BindMesh(m_meshes[2]);

		unsigned maxBufferSize = (unsigned(sizeof(GLR::PointLight)) + ((16 - (unsigned(sizeof(GLR::PointLight)) % 16)) % 16)) * 1024;
		for (unsigned i = 0; i < unsigned(ceil(float(pointLightBufferSize) / float(maxBufferSize))); i++)
		{
			m_shaders[2]->GetUniformBlock("PointLightBlock")->UpdateContents(&pointLightBuffer.get()[i * maxBufferSize], glm::min(maxBufferSize, pointLightBufferSize - maxBufferSize * i), 0);
			GLR::DrawIndexedIndirect(m_commandBuffers[0]);
		}

		GLR::SetRasterizationState(true, GL_BACK, GL_CCW);
#endif

		GLR::SetDepthState(true, true, GL_LESS);
		GLR::SetBlendState(false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GLR::UnbindShader();
	}

private:
	std::vector<GLR::Mesh> m_meshes;
	std::unique_ptr<GLR::Shader> m_shaders[3];
	Camera m_camera;
	std::unique_ptr<GLR::Framebuffer> m_framebuffer;
	std::vector<unsigned> m_commandBuffers;
	std::vector<GLR::PointLight> m_pointLights;
};

int main()
{
	TiledDeferredDemo TiledDeferredDemo(SCREEN_WIDTH, SCREEN_HEIGHT, "TiledDeferredDemo", false);
	TiledDeferredDemo.StartGameLoop();

	return 0;
}