#pragma once
#include "GL.h"
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "Mesh.h"

namespace GLR
{
	class Shader
	{
		struct InputParameter;
		struct UniformBlock;
	public:
		Shader(const char* computeShader);
		Shader(const char* vertexShader, const char* fragmentShader);
		Shader(const char* vertexShader, const char* geometryShader, const char* fragmentShader);
		Shader(const std::vector<const char*>& shaderFiles, const std::vector<GLenum>& shaderType);
		~Shader();

		static void AddGlobalUniformBlock(const std::string& uniformBlockName);
		void CheckCompatibilityMesh(const Mesh* mesh);

		void SetUniform(const std::string& name, const float& f);
		void SetUniform(const std::string& name, const glm::vec2& v);
		void SetUniform(const std::string& name, const glm::vec3& v);
		void SetUniform(const std::string& name, const glm::vec4& v);
		void SetUniform(const std::string& name, const int& i);
		void SetUniform(const std::string& name, const glm::ivec2& v);
		void SetUniform(const std::string& name, const glm::ivec3& v);
		void SetUniform(const std::string& name, const glm::ivec4& v);
		void SetUniform(const std::string& name, const unsigned& u);
		void SetUniform(const std::string& name, const glm::uvec2& v);
		void SetUniform(const std::string& name, const glm::uvec3& v);
		void SetUniform(const std::string& name, const glm::uvec4& v);
		void SetUniform(const std::string& name, const double& d);
		void SetUniform(const std::string& name, const glm::dvec2& v);
		void SetUniform(const std::string& name, const glm::dvec3& v);
		void SetUniform(const std::string& name, const glm::dvec4& v);
		//void SetUniform(const std::string& name, const Texture& t);

		GLuint GetProgram() const;

	private:
		static void CompileShader(GLuint& shaderID, GLenum shaderType, const char* shaderSource);
		static void LinkProgram(GLuint programID);
		static void ValidateProgram(GLuint programID);
		void LoadUniforms();
		void LoadAttributes();
		void LoadUniformBlocks();

		GLuint m_programID = 0;
		std::map<std::string, InputParameter> m_uniforms;
		std::map<std::string, InputParameter> m_attributes;
		std::map<std::string, UniformBlock> m_uniformBlocks;
		static std::vector<std::string> m_globalUniformBlocks;

		struct InputParameter
		{
			InputParameter() : name(""), location(-1), type(0), sampler(-1)
			{}
			InputParameter(const std::string& name, GLint location, GLenum type, GLint sampler = -1) : name(name), location(location), type(type), sampler(sampler)
			{}

			std::string name;
			GLint location;
			GLenum type;
			GLint sampler;
		};

		struct UniformBlock
		{
			UniformBlock() : name(""), bufferID(-1), binding(-1), size(0)
			{}
			UniformBlock(const std::string& name, GLint buffer, GLint binding, GLuint size) : name(name), bufferID(buffer), binding(binding), size(size)
			{}

			std::string name;
			GLint bufferID;
			GLint binding;
			GLuint size;
		};
	};
}
