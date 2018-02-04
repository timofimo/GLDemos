#include "PCH.h"
#include "Voxelizer.h"
#include "imgui/imgui.h"
#include "Shader.h"
#include "VXGIDemo.h"

#define VOXEL_RESOLUTION 64
#define VOXEL_SCALE 0.125f
#define VOXEL_CASCADES 5

Voxelizer::Voxelizer(): ISystem("Voxelizer")
{
	std::vector<std::string> defines = { "VOXEL_CASCADES 5" };
	m_voxelizationShader = std::make_unique<GLR::Shader>("Voxelization", "D:/Programming/GLDemos/VXGI/res/shaders/voxelization.vert", "D:/Programming/GLDemos/VXGI/res/shaders/voxelization.geom", "D:/Programming/GLDemos/VXGI/res/shaders/voxelization.frag", &defines);
	m_visualizationShader = std::make_unique<GLR::Shader>("VoxelVisualization", "D:/Programming/GLDemos/VXGI/res/shaders/voxelVisualization.vert", "D:/Programming/GLDemos/VXGI/res/shaders/voxelVisualization.geom", "D:/Programming/GLDemos/VXGI/res/shaders/voxelVisualization.frag");

	m_voxelDataAlbedo.reserve(VOXEL_CASCADES);
	for (int i = 0; i < VOXEL_CASCADES; i++)
		m_voxelDataAlbedo.emplace_back("voxelDataAlbedo" + std::to_string(i), VOXEL_RESOLUTION, VOXEL_RESOLUTION, VOXEL_RESOLUTION, GL_RGBA8);
	m_voxelDataFaces = std::make_unique<GLR::Image3D>("voxelDataFaces", VOXEL_RESOLUTION, VOXEL_RESOLUTION, VOXEL_RESOLUTION, GL_R32UI);

	std::vector<GLR::ColorAttachmentDescription> colorAttachments = { GLR::ColorAttachmentDescription{ 4, GL_UNSIGNED_BYTE } };
	m_voxelizationFB = std::make_unique<GLR::Framebuffer>("Voxelization", VOXEL_RESOLUTION, VOXEL_RESOLUTION, 8, colorAttachments);
}


Voxelizer::~Voxelizer()
{
}

void Voxelizer::DrawSettingsWindow()
{
	if (m_windowOpen && ImGui::Begin("Voxelizer", &m_windowOpen))
	{
		ImGui::Checkbox("Visualize", &m_settingVisualize);
		ImGui::End();
	}
}

void Voxelizer::Update(float deltatime, ExampleBase* base)
{
	VXGIDemo* demo = reinterpret_cast<VXGIDemo*>(base);

	if (!Input::GetKey(GLFW_KEY_LEFT_CONTROL))
	{
		glm::vec3 halfGridRes = glm::vec3(VOXEL_RESOLUTION * 0.5f * VOXEL_SCALE);
		for (int i = 0; i < VOXEL_CASCADES; i++)
		{
			float voxelScale = VOXEL_SCALE * float(1 << i);
			glm::ivec3 iCamPos = glm::floor(demo->m_camera.GetPosition() / voxelScale);
			iCamPos.x %= 2;
			iCamPos.y %= 2;
			iCamPos.z %= 2;
			glm::vec3 camPos = (glm::floor(demo->m_camera.GetPosition() / voxelScale) - glm::vec3(iCamPos)) * voxelScale;
			glm::mat4 translate = glm::translate(-camPos);
			glm::mat4 ortho = glm::ortho(-halfGridRes.x, halfGridRes.x, -halfGridRes.y, halfGridRes.y, -halfGridRes.z, halfGridRes.z);
			m_voxelGrid.projectionMatrix[0][i] = ortho * glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(-90.0f), glm::vec3(0, 0, 1)) * translate;
			m_voxelGrid.projectionMatrix[1][i] = ortho * glm::rotate(glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(90.0f), glm::vec3(0, 0, 1)) * translate;
			m_voxelGrid.projectionMatrix[2][i] = ortho * translate;
			m_voxelGrid.worldPosition[i] = glm::vec4(camPos - halfGridRes, voxelScale);
			halfGridRes *= 2.0f;
		}
		m_voxelGrid.resolution = VOXEL_RESOLUTION;
		m_voxelGrid.cascadeLevels = VOXEL_CASCADES;
	}
}

void Voxelizer::Render(ExampleBase* base)
{
	Voxelize(base);
	if (m_settingVisualize)
		VoxelVisualization(base);
}

void Voxelizer::Voxelize(ExampleBase* base)
{
	VXGIDemo* demo = reinterpret_cast<VXGIDemo*>(base);

	GLR::BindFramebuffer(*m_voxelizationFB);
	GLR::SetViewport(0, 0, VOXEL_RESOLUTION, VOXEL_RESOLUTION);
	GLR::SetColorMask(false, false, false, false);
	GLR::SetDepthState(false, false, GL_LESS);
	GLR::SetRasterizationState(false, GL_BACK, GL_CCW);
	GLR::SetMSAA(true);

	unsigned clearValue = 0x00000000;
	for (int i = 0; i < VOXEL_CASCADES; i++)
		m_voxelDataAlbedo[i].Clear(&clearValue, GL_UNSIGNED_BYTE);
	m_voxelDataFaces->Clear(&clearValue, GL_UNSIGNED_INT);

	GLR::BindShader(*m_voxelizationShader);

	m_voxelizationShader->GetUniform("voxelDataAlbedo")->SetArray(&m_voxelDataAlbedo[0], m_voxelDataAlbedo.size());
	m_voxelizationShader->GetUniform("voxelDataFaces")->Set(*m_voxelDataFaces);
	m_voxelizationShader->GetUniformBlock("VoxelGrid")->UpdateContents(reinterpret_cast<const char*>(&m_voxelGrid), sizeof(VoxelGrid), 0);
	demo->RenderAllMeshes(m_voxelizationShader.get());
	GLR::UnbindMesh();
	GLR::UnbindShader();

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Voxelizer::VoxelVisualization(ExampleBase* base)
{
	VXGIDemo* demo = reinterpret_cast<VXGIDemo*>(base);

	GLR::UnbindFramebuffer();
	GLR::SetViewport(0, 0, demo->GetWindowWidth(), demo->GetWindowHeight());
	GLR::SetColorMask(true, true, true, true);
	GLR::SetDepthState(true, true, GL_LESS);
	GLR::SetRasterizationState(true, GL_BACK, GL_CCW);

	GLR::BindShader(*m_visualizationShader);
	m_visualizationShader->GetUniform("voxelDataAlbedo")->SetArray(&m_voxelDataAlbedo[0], m_voxelDataAlbedo.size());
	m_visualizationShader->GetUniform("voxelDataFaces")->Set(*m_voxelDataFaces);

	GLR::BindMesh(demo->m_meshes[0]);	// Bind any vbo
	GLR::DrawPointsInstanced(1, 0, VOXEL_RESOLUTION * VOXEL_RESOLUTION * VOXEL_RESOLUTION * VOXEL_CASCADES);

	GLR::UnbindMesh();
	GLR::UnbindShader();
}
