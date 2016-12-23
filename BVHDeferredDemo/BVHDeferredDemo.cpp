#include "GLRenderer.h"
#include "Shader.h"
#include "Mesh.h"
#include <ExampleBase/ExampleBase.h>
#include "ResourceLoader.h"
#include "ExampleBase/Camera.h"
#include "Framebuffer.h"
#include "Light.h"
#include "PhysicsShapes.h"
#include "BVH.h"

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

class BVHDeferredDemo : public ExampleBase
{
public:
	BVHDeferredDemo(unsigned width, unsigned height, const char* title, bool borderless)
		: ExampleBase(width, height, title, borderless), m_camera(glm::radians(60.0f), float(width) / float(height), 0.1f, 2000.0f)
	{
		m_meshes.push_back(GLR::Mesh("Quad", vertexData, 12, indexData, 6, std::vector<GLenum>{ GL_FLOAT_VEC3}));
		GLR::ResourceLoader::LoadMeshes("D:/Programming/GLDemos/Resources/primitives.fbx", m_meshes, GLR::EBatchType::PerFile);
		GLR::ResourceLoader::LoadMeshes("D:/Programming/GLDemos/Resources/sponza/sponza.fbx", m_meshes, GLR::EBatchType::PerFile);

		GLR::Shader::AddGlobalUniformBlock("CameraBlock");
		m_shaders[1] = std::make_unique<GLR::Shader>("geometryShader", "D:/Programming/GLDemos/BVHDeferredDemo/res/shaders/geometryPass.vert", "D:/Programming/GLDemos/BVHDeferredDemo/res/shaders/geometryPass.frag");
		m_shaders[2] = std::make_unique<GLR::Shader>("lightShader", "D:/Programming/GLDemos/BVHDeferredDemo/res/shaders/lightPassAdvanced.vert", "D:/Programming/GLDemos/BVHDeferredDemo/res/shaders/lightPassAdvanced.frag");

		GLR::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		GLR::SetViewport(0, 0, width, height);
		GLR::SetDepthState(true, true, GL_LESS);
		GLR::SetRasterizationState(true, GL_BACK, GL_CCW);

		m_camera.SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));

		m_framebuffer = std::make_unique<GLR::Framebuffer>("TestFBO", width, height, std::vector<GLR::ColorAttachmentDescription>{ {3, GL_UNSIGNED_BYTE}, {3, GL_UNSIGNED_BYTE} }, GL_DEPTH_COMPONENT32F);

		m_pointLights.reserve(1024);
		for (unsigned i = 0; i < 1024; i++)
		{
			m_pointLights.emplace_back(glm::vec3(((rand() % 360) - 180) * 0.08f, (rand() % 140) * 0.08f, ((rand() % 220) - 110) * 0.08f), glm::vec4((rand() % 255) * 0.004f, (rand() % 255) * 0.004f, (rand() % 255) * 0.004f, 0.01f), 1.0f, 0.0f, 0.0f);
		}

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
	~BVHDeferredDemo()
	{
	}

	virtual void Update(float deltaTime) override
	{
		m_camera.Update(deltaTime);
	}

	virtual void Render() override
	{
		double start = glfwGetTime();
		std::vector<GLR::BBox> plBoundingBoxes;
		plBoundingBoxes.reserve(m_pointLights.size());
		for (unsigned i = 0; i < unsigned(m_pointLights.size()); i++)
			plBoundingBoxes.push_back(GLR::BBox(m_pointLights[i].GetPosition(), m_pointLights[i].GetRange()));
		double listDelta = glfwGetTime() - start;

		start = glfwGetTime();
		GLR::BVH bvh(plBoundingBoxes, 1);
		double BVH = glfwGetTime() - start;

		start = glfwGetTime();
		std::vector<unsigned> result = bvh.Intersect(glm::vec3(glm::sin(glfwGetTime()) * 3.0f, 0.0f, glm::cos(glfwGetTime()) * 3.0f));
		double test = glfwGetTime() - start;

		//printf_s("List construction: %fms   BVH construction: %f   Intersection test: %f   Total: %f\n", listDelta * 1000.0, BVH * 1000.0, test * 1000.0, (listDelta + BVH + test) * 1000.0);

		// Geometry pass
		GLR::BindShader(*m_shaders[1]);
		GLR::BindFramebuffer(*m_framebuffer);

		struct CameraBlock
		{
			glm::mat4 viewProjectionMatrix;
			glm::mat4 invViewProjectionMatrix;
			glm::vec3 position;
			float padding;
		} cameraBlock;
		cameraBlock.viewProjectionMatrix = m_camera.GetViewProjectionMatrix();
		cameraBlock.invViewProjectionMatrix = glm::inverse(cameraBlock.viewProjectionMatrix);
		cameraBlock.position = m_camera.GetWorldPosition();
		m_shaders[1]->GetUniformBlock("CameraBlock")->UpdateContents(reinterpret_cast<char*>(&cameraBlock), sizeof(CameraBlock), 0);

		GLR::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		const GLR::InputParameter* modelMatrix = m_shaders[1]->GetUniform("modelMatrix");
		const GLR::InputParameter* meshColor = m_shaders[1]->GetUniform("meshColor");
		meshColor->Set(glm::vec3(1.0f, 1.0f, 1.0f));

		for (unsigned j = 0; j < 1; j++)
		{
			modelMatrix->Set(glm::translate(glm::vec3(j * 64.0f, 0.0f, 0.0f)));
			GLR::BindMesh(m_meshes[3]);
			GLR::DrawIndexedIndirect(m_commandBuffers[3]);
		}

		GLR::BindMesh(m_meshes[1]);
		for (unsigned j = 0; j < unsigned(result.size()); j++)
		{
			GLR::BBox box = bvh.Get(result[j]);
			modelMatrix->Set(glm::translate(box.Center()) * glm::scale(box.Max() - box.Center()));
			GLR::DrawLinesIndexed(m_meshes[1].GetIndexCount(), m_meshes[1].GetIndexOffset());
		}

		GLR::UnbindShader();

		// Light pass
		GLR::UnbindFramebuffer();
		GLR::SetBlendState(true, GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
		GLR::SetDepthState(false, true, GL_LESS);
		GLR::BindShader(*m_shaders[2]);

		m_shaders[2]->GetUniform("colorSampler")->Set(*GLR::Texture2D::GetItem("TestFBO_0"));
		m_shaders[2]->GetUniform("normalSampler")->Set(*GLR::Texture2D::GetItem("TestFBO_1"));
		m_shaders[2]->GetUniform("depthSampler")->Set(*GLR::Texture2D::GetItem("TestFBO_depth"));

		std::shared_ptr<char> pointLightBuffer;
		unsigned pointLightBufferSize;
		GLR::Light<GLR::PointLight>::GetBuffer(pointLightBuffer, pointLightBufferSize);

		/*unsigned plSize = sizeof(GLR::PointLight) + (16 - (sizeof(GLR::PointLight) % 16));
		for (unsigned i = 0; i < (pointLightBufferSize / plSize); i++)
		{
			GLR::PointLight* pl = reinterpret_cast<GLR::PointLight*>(&pointLightBuffer.get()[i * plSize]);
			pl->SetPosition(pl->GetPosition() + glm::vec3(glm::sin(glfwGetTime()) * glm::sign(int(i % 6) - 3), 0.0f, glm::cos(glfwGetTime()) * glm::sign(int(i % 12) - 4)) * 4.0f);
		}*/

		GLR::SetRasterizationState(true, GL_FRONT, GL_CCW);
		GLR::BindMesh(m_meshes[2]);

		unsigned maxBufferSize = (unsigned(sizeof(GLR::PointLight)) + (16 - (unsigned(sizeof(GLR::PointLight)) % 16))) * 1024;
		for (unsigned i = 0; i < unsigned(ceil(float(pointLightBufferSize) / float(maxBufferSize))); i++)
		{
			m_shaders[2]->GetUniformBlock("PointLightBlock")->UpdateContents(&pointLightBuffer.get()[i * maxBufferSize], glm::min(maxBufferSize, pointLightBufferSize - maxBufferSize * i), 0);
			GLR::DrawIndexedIndirect(m_commandBuffers[0]);
		}

		GLR::SetRasterizationState(true, GL_BACK, GL_CCW);
		GLR::SetDepthState(true, true, GL_LESS);
		GLR::SetBlendState(false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GLR::UnbindShader();

		GLR::BindShader(*m_shaders[1]);
		meshColor->Set(glm::vec3(1.0f, 1.0f, 1.0f));
		GLR::BindMesh(m_meshes[1]);
		for (unsigned j = 0; j < unsigned(result.size()); j++)
		{
			GLR::BBox box = bvh.Get(result[j]);
			modelMatrix->Set(glm::translate(box.Center()) * glm::scale(box.Max() - box.Center()));
			GLR::DrawLinesIndexed(m_meshes[1].GetIndexCount(), m_meshes[1].GetIndexOffset());
		}
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
	BVHDeferredDemo BVHDeferredDemo(640, 480, "BVHDeferredDemo", false);
	BVHDeferredDemo.StartGameLoop();

	return 0;
}