#pragma once
#include "GL.h"
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Utilities.h"
#include "Texture.h"
#include "GLRenderer.h"

namespace GLR
{
	struct InputParameter
	{
		InputParameter() : name(""), location(-1), type(0), sampler(-1)
		{}
		InputParameter(const std::string& name, GLint location, GLenum type, GLint sampler = -1) : name(name), location(location), type(type), sampler(sampler)
		{}

		void Set(const float& f) const
		{
			assert(type == GL_FLOAT && "The uniform type doesn't match the input type");
			glUniform1f(location, f);
			GL_GET_ERROR();
		}
		void Set(const glm::vec2& v) const
		{
			assert(type == GL_FLOAT_VEC2 && "The uniform type doesn't match the input type");
			glUniform2fv(location, 1, &v[0]);
			GL_GET_ERROR();
		}
		void Set(const glm::vec3& v) const
		{
			assert(type == GL_FLOAT_VEC3 && "The uniform type doesn't match the input type");
			glUniform3fv(location, 1, &v[0]);
			GL_GET_ERROR();
		}
		void Set(const glm::vec4& v) const
		{
			assert(type == GL_FLOAT_VEC4 && "The uniform type doesn't match the input type");
			glUniform4fv(location, 1, &v[0]);
			GL_GET_ERROR();
		}
		void Set(const int& i) const
		{
			assert(type == GL_INT && "The uniform type doesn't match the input type");
			glUniform1i(location, i);
			GL_GET_ERROR();
		}
		void Set(const glm::ivec2& v) const
		{
			assert(type == GL_INT_VEC2 && "The uniform type doesn't match the input type");
			glUniform2iv(location, 1, &v[0]);
			GL_GET_ERROR();
		}
		void Set(const glm::ivec3& v) const
		{
			assert(type == GL_INT_VEC3 && "The uniform type doesn't match the input type");
			glUniform3iv(location, 1, &v[0]);
			GL_GET_ERROR();
		}
		void Set(const glm::ivec4& v) const
		{
			assert(type == GL_INT_VEC4 && "The uniform type doesn't match the input type");
			glUniform4iv(location, 1, &v[0]);
			GL_GET_ERROR();
		}
		void Set(const unsigned& u) const
		{
			assert(type == GL_UNSIGNED_INT && "The uniform type doesn't match the input type");
			glUniform1ui(location, u);
			GL_GET_ERROR();
		}
		void Set(const glm::uvec2& v) const
		{
			assert(type == GL_UNSIGNED_INT_VEC2 && "The uniform type doesn't match the input type");
			glUniform2uiv(location, 1, &v[0]);
			GL_GET_ERROR();
		}
		void Set(const glm::uvec3& v) const
		{
			assert(type == GL_UNSIGNED_INT_VEC3 && "The uniform type doesn't match the input type");
			glUniform3uiv(location, 1, &v[0]);
			GL_GET_ERROR();
		}
		void Set(const glm::uvec4& v) const
		{
			assert(type == GL_UNSIGNED_INT_VEC4 && "The uniform type doesn't match the input type");
			glUniform4uiv(location, 1, &v[0]);
			GL_GET_ERROR();
		}
		void Set(const double& d) const
		{
			assert(type == GL_DOUBLE && "The uniform type doesn't match the input type");
			glUniform1d(location, d);
			GL_GET_ERROR();
		}
		void Set(const glm::dvec2& v) const
		{
			assert(type == GL_DOUBLE_VEC2 && "The uniform type doesn't match the input type");
			glUniform2dv(location, 1, &v[0]);
			GL_GET_ERROR();
		}
		void Set(const glm::dvec3& v) const
		{
			assert(type == GL_DOUBLE_VEC3 && "The uniform type doesn't match the input type");
			glUniform3dv(location, 1, &v[0]);
			GL_GET_ERROR();
		}
		void Set(const glm::dvec4& v) const
		{
			assert(type == GL_DOUBLE_VEC4 && "The uniform type doesn't match the input type");
			glUniform4dv(location, 1, &v[0]);
			GL_GET_ERROR();
		}
		void Set(const glm::mat4& m) const
		{
			assert(type == GL_FLOAT_MAT4 && "The uniform type doesn't match the input type");
			glUniformMatrix4fv(location, 1, false, &m[0][0]);
			GL_GET_ERROR();
		}
		void Set(const Texture2D& t) const
		{
			assert(type == GL_SAMPLER_2D && "The uniform type doesn't match the input type");
			BindTexture(t, sampler);
			glUniform1i(location, sampler);
			GL_GET_ERROR();
		}

		std::string name;
		GLint location;
		GLenum type;
		GLint sampler;
	};

	struct UniformBlock
	{
		UniformBlock() : name(""), bufferID(-1), binding(-1), bufferSize(0)
		{}
		UniformBlock(const std::string& name, GLint buffer, GLint binding, GLuint size) : name(name), bufferID(buffer), binding(binding), bufferSize(size)
		{}

		void UpdateContents(const char* data, unsigned size, unsigned offset) const
		{
			glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
			glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
			GL_GET_ERROR();
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		std::string name;
		GLint bufferID;
		GLint binding;
		GLuint bufferSize;
	};

	struct ShaderStorageBlock
	{
		ShaderStorageBlock() : name(""), bufferID(-1), binding(-1), bufferSize(0)
		{}
		ShaderStorageBlock(const std::string& name, GLint buffer, GLint binding) : name(name), bufferID(buffer), binding(binding), bufferSize(0)
		{}

		void UpdateContents(const char* data, unsigned size)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
			if (size > bufferSize)
			{
				glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STREAM_DRAW);
				GL_GET_ERROR();
				bufferSize = size;
			}
			else
			{
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, data);
				GL_GET_ERROR();
			}
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		void Reserve(unsigned size)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
			if (size > bufferSize)
			{
				glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_STREAM_DRAW);
				GL_GET_ERROR();
				bufferSize = size;
			}
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		std::string name;
		GLint bufferID;
		GLint binding;
		GLuint bufferSize;
	};

	class Shader : public ManagedItem<Shader>
	{
	public:
		Shader(const std::string& name, const char* computeShader);
		Shader(const std::string& name, const char* vertexShader, const char* fragmentShader);
		Shader(const std::string& name, const char* vertexShader, const char* geometryShader, const char* fragmentShader);
		Shader(const std::string& name, const std::vector<const char*>& shaderFiles, const std::vector<GLenum>& shaderType);
		~Shader();

		static void AddGlobalUniformBlock(const std::string& uniformBlockName);
		static void AddGlobalShaderStorageBlock(const std::string& shaderStorageBlockName);
		void CheckCompatibilityMesh(const Mesh* mesh);

		GLuint GetProgram() const;
		const InputParameter* GetUniform(const std::string& name) const;
		const UniformBlock* GetUniformBlock(const std::string& name) const;
		ShaderStorageBlock* GetShaderStorageBlock(const std::string& name);

	private:
		static void CompileShader(GLuint& shaderID, GLenum shaderType, const char* shaderSource);
		static void LinkProgram(GLuint programID);
		static void ValidateProgram(GLuint programID);
		void LoadUniforms();
		void LoadAttributes();
		void LoadUniformBlocks();
		void LoadShaderStorageBlocks();

		GLuint m_programID = 0;
		std::map<std::string, InputParameter> m_uniforms;
		std::map<std::string, InputParameter> m_attributes;
		std::map<std::string, UniformBlock> m_uniformBlocks;
		static std::map<std::string, UniformBlock> m_globalUniformBlocks;
		std::map<std::string, ShaderStorageBlock> m_shaderStorageBlocks;
		static std::map<std::string, ShaderStorageBlock> m_globalShaderStorageBlocks;
		static unsigned m_uniformBindingOffset;
		static unsigned m_shaderStorageBindingOffset;
	};
}
