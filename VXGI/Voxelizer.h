#pragma once
#include "ISystem.h"
#include "Image3D.h"
#include "Framebuffer.h"
#include "Shader.h"

struct VoxelGrid
{
	glm::mat4 projectionMatrix[3][5];
	glm::vec4 worldPosition[5];
	unsigned resolution;
	unsigned cascadeLevels;
};

class Voxelizer : public ISystem
{
public:
	Voxelizer();
	~Voxelizer();

	void DrawSettingsWindow() override;
	void Update(float deltatime, ExampleBase* base) override;
	void Render(ExampleBase* base) override;

private:
	void Voxelize(ExampleBase* base);
	void VoxelVisualization(ExampleBase* base);

	std::unique_ptr<GLR::Shader> m_voxelizationShader, m_visualizationShader;
	std::vector<GLR::Image3D> m_voxelDataAlbedo;
	std::unique_ptr<GLR::Image3D> m_voxelDataFaces;
	std::unique_ptr<GLR::Framebuffer> m_voxelizationFB;
	VoxelGrid m_voxelGrid;

	bool m_settingVisualize = false;
};

