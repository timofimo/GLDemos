#include "..\include\Model.h"
#include "Shader.h"


GLR::Model::Model(Shader* shader, Mesh* mesh)
{
	m_shader = shader;
	m_mesh = mesh;

	m_shader->CheckCompatibilityMesh(mesh);
}
