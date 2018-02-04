#pragma once
#include "Mesh.h"
//#include "Texture.h"
#include "GLRenderer.h"
#include "Image3D.h"

class Texture2D;

namespace GLR
{
	struct InputParameter
	{
		InputParameter() : name(""), location(-1), type(0), arraySize(0), sampler(-1)
		{}
		InputParameter(const std::string& name, GLint location, GLenum type, int arraySize, GLint sampler = -1) : name(name), location(location), type(type), arraySize(arraySize), sampler(sampler)
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
		void Set(const Image3D& t) const
		{
			assert((type == GL_IMAGE_3D || type == GL_INT_IMAGE_3D || type == GL_UNSIGNED_INT_IMAGE_3D) && "The uniform type doesn't match the input type");
			BindImage(t, sampler);
			glUniform1i(location, sampler);
			GL_GET_ERROR();
		}

		void SetArray(const float* f, int count) const
		{
			assert(type == GL_FLOAT && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform1fv(location, count, f);
			GL_GET_ERROR();
		}
		void SetArray(const glm::vec2* v, int count) const
		{
			assert(type == GL_FLOAT_VEC2 && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform2fv(location, count, &v[0][0]);
			GL_GET_ERROR();
		}
		void SetArray(const glm::vec3* v, int count) const
		{
			assert(type == GL_FLOAT_VEC3 && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform3fv(location, count, &v[0][0]);
			GL_GET_ERROR();
		}
		void SetArray(const glm::vec4* v, int count) const
		{
			assert(type == GL_FLOAT_VEC4 && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform4fv(location, count, &v[0][0]);
			GL_GET_ERROR();
		}
		void SetArray(const int* i, int count) const
		{
			assert(type == GL_INT && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform1iv(location, count, i);
			GL_GET_ERROR();
		}
		void SetArray(const glm::ivec2* v, int count) const
		{
			assert(type == GL_INT_VEC2 && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform2iv(location, count, &v[0][0]);
			GL_GET_ERROR();
		}
		void SetArray(const glm::ivec3* v, int count) const
		{
			assert(type == GL_INT_VEC3 && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform3iv(location, count, &v[0][0]);
			GL_GET_ERROR();
		}
		void SetArray(const glm::ivec4* v, int count) const
		{
			assert(type == GL_INT_VEC4 && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform4iv(location, count, &v[0][0]);
			GL_GET_ERROR();
		}
		void SetArray(const unsigned* u, int count) const
		{
			assert(type == GL_UNSIGNED_INT && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform1uiv(location, count, u);
			GL_GET_ERROR();
		}
		void SetArray(const glm::uvec2* v, int count) const
		{
			assert(type == GL_UNSIGNED_INT_VEC2 && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform2uiv(location, count, &v[0][0]);
			GL_GET_ERROR();
		}
		void SetArray(const glm::uvec3* v, int count) const
		{
			assert(type == GL_UNSIGNED_INT_VEC3 && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform3uiv(location, count, &v[0][0]);
			GL_GET_ERROR();
		}
		void SetArray(const glm::uvec4* v, int count) const
		{
			assert(type == GL_UNSIGNED_INT_VEC4 && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform4uiv(location, count, &v[0][0]);
			GL_GET_ERROR();
		}
		void SetArray(const double* d, int count) const
		{
			assert(type == GL_DOUBLE && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform1dv(location, count, d);
			GL_GET_ERROR();
		}
		void SetArray(const glm::dvec2* v, int count) const
		{
			assert(type == GL_DOUBLE_VEC2 && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform2dv(location, count, &v[0][0]);
			GL_GET_ERROR();
		}
		void SetArray(const glm::dvec3* v, int count) const
		{
			assert(type == GL_DOUBLE_VEC3 && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform3dv(location, count, &v[0][0]);
			GL_GET_ERROR();
		}
		void SetArray(const glm::dvec4* v, int count) const
		{
			assert(type == GL_DOUBLE_VEC4 && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniform4dv(location, count, &v[0][0]);
			GL_GET_ERROR();
		}
		void SetArray(const glm::mat4* m, int count) const
		{
			assert(type == GL_FLOAT_MAT4 && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			glUniformMatrix4fv(location, count, false, &m[0][0][0]);
			GL_GET_ERROR();
		}
		void SetArray(const Texture2D* t, int count) const
		{
			assert(type == GL_SAMPLER_2D && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			BindTextures(t, count, sampler);
			for (int i = 0; i < count; i++)
				glUniform1i(location + i, sampler);
			GL_GET_ERROR();
		}
		void SetArray(const Image3D* t, int count) const
		{
			assert((type == GL_IMAGE_3D || type == GL_INT_IMAGE_3D || type == GL_UNSIGNED_INT_IMAGE_3D) && "The uniform type doesn't match the input type");
			assert(count <= arraySize && "The count is too high");
			BindImages(t, count, sampler);
			for (int i = 0; i < count; i++)
				glUniform1i(location + i, sampler + i);
			GL_GET_ERROR();
		}

		std::string name;
		GLint location;
		GLenum type;
		int arraySize;
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

	struct Image
	{
		Image() : name(""), imageID(-1), binding(-1), bufferSize(0)
		{}
		Image(const std::string& name, GLint binding, GLuint size) : name(name), imageID(-1), binding(binding), bufferSize(size)
		{}

		void Bind(const Image3D& image)
		{
			GLuint id = image.GetImageID();
			assert(id != 0 && "Image3D hasn't been initialized");

			if (id != imageID)
			{
				glBindImageTexture(binding, id, 0, GL_FALSE, 0, GL_RED_INTEGER, GL_RGBA8);
				GL_GET_ERROR();
				imageID = id;
			}
		}

		std::string name;
		GLuint imageID;
		GLint binding;
		GLuint bufferSize;
	};

	class Shader : public ManagedItem<Shader>
	{
	public:
		Shader(const std::string& name, const char* computeShader, const std::vector<std::string>* defines = nullptr);
		Shader(const std::string& name, const char* vertexShader, const char* fragmentShader, const std::vector<std::string>* defines = nullptr);
		Shader(const std::string& name, const char* vertexShader, const char* geometryShader, const char* fragmentShader, const std::vector<std::string>* defines = nullptr);
		Shader(const std::string& name, const std::vector<const char*>& shaderFiles, const std::vector<GLenum>& shaderType, const std::vector<std::string>* defines = nullptr);
		~Shader();

		static void AddGlobalUniformBlock(const std::string& uniformBlockName);
		static void AddGlobalShaderStorageBlock(const std::string& shaderStorageBlockName);
		void CheckCompatibilityMesh(const Mesh* mesh);

		GLuint GetProgram() const;
		const InputParameter* GetUniform(const std::string& name) const;
		const UniformBlock* GetUniformBlock(const std::string& name) const;
		ShaderStorageBlock* GetShaderStorageBlock(const std::string& name);
		static const UniformBlock* GetGlobalUniformBlock(const std::string& name);
		static ShaderStorageBlock* GetGlobalShaderStorageBlock(const std::string& name);

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
