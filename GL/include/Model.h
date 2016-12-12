#pragma once

namespace GLR
{
	class Mesh;
	class Shader;

	class Model
	{
	public:
		Model(Shader* shader, Mesh* mesh);
		~Model();

	private:
		Shader* m_shader;
		Mesh* m_mesh;
	};
}
