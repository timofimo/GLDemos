#pragma once
#include "ISystem.h"
#include "Material.h"
#include "ExampleBase/Camera.h"
#include <ExampleBase\ExampleBase.h>

struct CameraBlock
{
	glm::mat4 viewProjectionMatrix;
};

namespace GLR {
	class Shader;
}

class VXGIDemo : public ExampleBase
{
public:
	VXGIDemo(unsigned width, unsigned height, const char* title, bool borderless);
	~VXGIDemo();

	virtual void Update(float deltaTime) override;
	virtual void Render() override;

	void UpdateCameraBuffer();
	void RenderUnlit();
	void RenderAllMeshes(const GLR::Shader* shader);

	std::vector<ISystem*> m_systems;
	std::unique_ptr<Voxelizer> m_voxelizer;
	std::vector<GLR::Mesh> m_meshes;
	std::vector<GLR::Material> m_materials;
	std::vector<unsigned> m_materialIndices;
	std::unique_ptr<GLR::Shader> m_unlitShader;
	Camera m_camera;
	CameraBlock m_cameraBlock;
};
