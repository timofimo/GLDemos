#include <PCH.h>
#include "GLRenderer.h"
#include "Shader.h"
#include "Mesh.h"
#include <ExampleBase/ExampleBase.h>
#include "ResourceLoader.h"
#include "ExampleBase/Camera.h"
#include "Material.h"
#include "imgui\imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "ISystem.h"
#include "Voxelizer.h"
#include "VXGIDemo.h"

VXGIDemo::VXGIDemo(unsigned width, unsigned height, const char* title, bool borderless)
	: ExampleBase(width, height, title, borderless), m_camera(glm::radians(60.0f), float(width) / float(height), 0.1f, 10000.0f)
{
	GLR::Shader::AddGlobalUniformBlock("CameraBlock");
	GLR::Shader::AddGlobalUniformBlock("VoxelGrid");

	m_systems.push_back((m_voxelizer = std::make_unique<Voxelizer>()).get());

	m_unlitShader = std::make_unique<GLR::Shader>("Unlit", "D:/Programming/GLDemos/VXGI/res/shaders/unlit.vert", "D:/Programming/GLDemos/VXGI/res/shaders/unlit.frag");

	GLR::ResourceLoader::LoadScene("D:/Programming/GLDemos/Resources/sponza/sponza.fbx", m_meshes, GLR::EBatchType::PerFile, m_materials, m_materialIndices);

	GLR::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	GLR::SetViewport(0, 0, width, height);
	GLR::SetDepthState(true, true, GL_LESS);
	GLR::SetRasterizationState(true, GL_BACK, GL_CCW);

	m_camera.SetPosition(glm::vec3(0.0f, 0.25f, 0.0f));

	ImGui_ImplGlfwGL3_Init(GetWindowHandle(), false);
}

VXGIDemo::~VXGIDemo()
{
	ImGui_ImplGlfwGL3_Shutdown();
}

void VXGIDemo::Update(float deltaTime)
{
	m_camera.Update(deltaTime);

	for (int i = 0; i < m_systems.size(); i++)
		m_systems[i]->Update(deltaTime, this);
}

void VXGIDemo::Render()
{
	ImGui_ImplGlfwGL3_NewFrame();
	UpdateCameraBuffer();

	for (int i = 0; i < m_systems.size(); i++)
		m_systems[i]->DrawSettingsWindow();

	for (int i = 0; i < m_systems.size(); i++)
		m_systems[i]->Render(this);

	RenderUnlit();
	
	if (ImGui::BeginMainMenuBar())
	{
		for (int i = 0; i < m_systems.size(); i++)
		{
			if (ImGui::Button(m_systems[i]->GetName().c_str()))
				m_systems[i]->OpenSettingsWindow();
		}
		ImGui::EndMainMenuBar();
	}

	ImGui::Render();
}

void VXGIDemo::UpdateCameraBuffer()
{
	m_cameraBlock.viewProjectionMatrix = m_camera.GetViewProjectionMatrix();
	GLR::Shader::GetGlobalUniformBlock("CameraBlock")->UpdateContents(reinterpret_cast<const char*>(&m_cameraBlock), sizeof(CameraBlock), 0);
}

void VXGIDemo::RenderUnlit()
{
	GLR::UnbindFramebuffer();
	GLR::SetViewport(0, 0, GetWindowWidth(), GetWindowHeight());
	GLR::SetColorMask(true, true, true, true);
	GLR::SetDepthState(true, true, GL_LESS);
	GLR::SetRasterizationState(true, GL_BACK, GL_CCW);

	GLR::BindShader(*m_unlitShader);
	RenderAllMeshes(m_unlitShader.get());
	GLR::UnbindMesh();
	GLR::UnbindShader();
}

void VXGIDemo::RenderAllMeshes(const GLR::Shader* shader)
{
	shader->GetUniform("modelMatrix")->Set(glm::scale(glm::vec3(0.01f)));

	for (unsigned i = 0; i < m_meshes.size(); i++)
	{
		GLR::BindMesh(m_meshes[i]);
		shader->GetUniformBlock("Material")->UpdateContents(reinterpret_cast<const char*>(&m_materials[m_materialIndices[i]]), sizeof(GLR::MaterialBuffer), 0);

		GLR::DrawIndexed(m_meshes[i].GetIndexCount(), m_meshes[i].GetIndexOffset());
	}

	shader->GetUniform("modelMatrix")->Set(glm::translate(glm::vec3(0.0f, -0.1f, 0.0f)) * glm::scale(glm::vec3(0.1f)));

	for (unsigned i = 0; i < m_meshes.size(); i++)
	{
		GLR::BindMesh(m_meshes[i]);
		shader->GetUniformBlock("Material")->UpdateContents(reinterpret_cast<const char*>(&m_materials[m_materialIndices[i]]), sizeof(GLR::MaterialBuffer), 0);

		GLR::DrawIndexed(m_meshes[i].GetIndexCount(), m_meshes[i].GetIndexOffset());
	}
}

int main()
{
	VXGIDemo VXGIDemo(1280, 720, "VXGIDemo", false);
	VXGIDemo.StartGameLoop();

	return 0;
}