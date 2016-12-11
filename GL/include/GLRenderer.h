#pragma once
#include <glad/glad.h>


namespace GLR
{
	class Shader;
	class Mesh;

	enum class EAttributeLayout
	{
		eFloat, eVec2, eVec3, eVec4,
		eInt, eIVec2, eIVec3, eIVec4,
		eUInt, eUVec2, eUVec3, eUVec4,
		eDouble, eDVec2, eDVec3, eDVec4,
		eMat3x3, eMat4x4,
	};

	namespace Internal
	{
		struct RendererState;
	}

	void Clear(GLbitfield mask);
	void DrawIndexed(unsigned count);

	void BindMesh(const Mesh& mesh);
	void BindShader(const Shader& shader);

	void SetClearColor(float r, float g, float b, float a);
}
