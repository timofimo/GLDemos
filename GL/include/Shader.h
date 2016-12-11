#pragma once
#include "GL.h"
#include <vector>
#include <map>

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
