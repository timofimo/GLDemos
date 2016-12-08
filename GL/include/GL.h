#pragma once
#include "Window.h"
#include <vector>

namespace GL
{
	enum class EAttributeLayout
	{
		eFloat, eVec2, eVec3, eVec4,
		eInt, eIVec2, eIVec3, eIVec4,
		eUInt, eUVec2, eUVec3, eUVec4,
		eDouble, eDVec2, eDVec3, eDVec4,
		eMat3x3, eMat4x4,
	};

	Mesh CreateMesh(const float* vertexData, const std::vector<VertexConfiguration>& vertexConfiguration, const std::vector<unsigned>& indexData = std::vector<unsigned>());
	Shader CreateShader(const char* vertexShader, const char* fragmentShader);

	void Clear(float r, float g, float b, float a);
	void DrawIndexed(unsigned count);

	void BindMesh(const Mesh& mesh);
	void BindShader(const Shader& shader);

}
