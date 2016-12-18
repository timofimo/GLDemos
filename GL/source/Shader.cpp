#include "..\include\Shader.h"
#include <assert.h>
#include "Utilities.h"
#include <algorithm>

std::vector<std::string> GLR::Shader::m_globalUniformBlocks;

GLR::Shader::Shader(const std::string& name, const char* computeShader) : Shader(name, {computeShader}, {GL_COMPUTE_SHADER})
{
}

GLR::Shader::Shader(const std::string& name, const char* vertexShader, const char* fragmentShader) : Shader(name, {vertexShader, fragmentShader}, {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER})
{
}

GLR::Shader::Shader(const std::string& name, const char* vertexShader, const char* geometryShader, const char* fragmentShader) : Shader(name, {vertexShader, geometryShader, fragmentShader}, {GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER})
{
}

/**
 * \brief Creates a shader program with the given GLSL files and shader types.
 * \param shaderFiles GLSL file location
 * \param shaderType Shader type <GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, ...>
 */
GLR::Shader::Shader(const std::string& name, const std::vector<const char*>& shaderFiles, const std::vector<GLenum>& shaderType) : ManagedItem(name, this)
{
#ifndef NDEBUG
	if (shaderFiles.size() != shaderType.size())
		LOG_E("Each shader file should have a corresponding shader type");

	unsigned shaderBits = 0;
	for(GLenum type : shaderType)
	{
		unsigned tempShaderBits = shaderBits;
		switch(type)
		{
		case GL_VERTEX_SHADER:
			tempShaderBits |= GL_VERTEX_SHADER_BIT;
			break;
		case GL_FRAGMENT_SHADER:
			tempShaderBits |= GL_FRAGMENT_SHADER_BIT;
			break;
		case GL_GEOMETRY_SHADER:
			tempShaderBits |= GL_GEOMETRY_SHADER_BIT;
			break;
		case GL_TESS_CONTROL_SHADER:
			tempShaderBits |= GL_TESS_CONTROL_SHADER_BIT;
			break;
		case GL_TESS_EVALUATION_SHADER:
			tempShaderBits |= GL_TESS_EVALUATION_SHADER_BIT;
			break;
		case GL_COMPUTE_SHADER:
			tempShaderBits |= (GL_TESS_EVALUATION_SHADER_BIT << 1);
		default: break;
		}

		if (shaderBits == tempShaderBits)
			LOG_E("Shader type should be unique");

		shaderBits = tempShaderBits;
	}
#endif

	// Create the shader program
	m_programID = glCreateProgram();

	// Compile the shaders and attach them to the program
	std::vector<GLuint> shaderIDs(shaderFiles.size());
	for(unsigned i = 0; i < unsigned(shaderIDs.size()); i++)
	{
		char* shaderSource;
		unsigned shaderSourceSize;
		ReadFile(shaderFiles[i], shaderSource, shaderSourceSize);
		CompileShader(shaderIDs[i], shaderType[i], shaderSource);

		glAttachShader(m_programID, shaderIDs[i]);
	}

	// Link the program
	LinkProgram(m_programID);

	// Delete the shaders
	for (unsigned i = 0; i < unsigned(shaderIDs.size()); i++)
	{
		glDeleteShader(shaderIDs[i]);
		GL_GET_ERROR();
	}
	shaderIDs.clear();

	// Get shader attribute and uniform information
	LoadAttributes();
	LoadUniforms();
	LoadUniformBlocks();
}

GLR::Shader::~Shader()
{
	if (m_programID != 0)
	{
		glDeleteProgram(m_programID);
		m_programID = 0;
	}
}

void GLR::Shader::AddGlobalUniformBlock(const std::string& uniformBlockName)
{
	m_globalUniformBlocks.push_back(uniformBlockName);
	std::sort(m_globalUniformBlocks.begin(), m_globalUniformBlocks.end());
}

void GLR::Shader::CheckCompatibilityMesh(const Mesh* mesh)
{
	std::vector<GLenum> meshAttributes = mesh->GetAttributes();
	if (m_attributes.size() != meshAttributes.size())
		LOG_E("Mesh and shader attributes do not have the same count");

	unsigned i = 0;
	for(auto it = m_attributes.begin(); it != m_attributes.end(); ++it)
	{
		if (it->second.type != meshAttributes[i])
			LOG_E("Mesh and shader attributes do not align");
		++i;
	}
}

GLuint GLR::Shader::GetProgram() const
{
	return m_programID;
}

const GLR::InputParameter* GLR::Shader::GetUniform(const std::string& name) const
{
	auto it = m_uniforms.find(name);
	if (it != m_uniforms.end())
		return &it->second;

	LOG_E("Uniform not found");
}

const GLR::UniformBlock* GLR::Shader::GetUniformBlock(const std::string& name) const
{
	auto it = m_uniformBlocks.find(name);
	if (it != m_uniformBlocks.end())
		return &it->second;

	LOG_E("Uniform block not found");
}

void GLR::Shader::CompileShader(GLuint& shaderID, GLenum shaderType, const char* shaderSource)
{
	GLint status;

	shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &shaderSource, nullptr);
	GL_GET_ERROR();
	glCompileShader(shaderID);
	GL_GET_ERROR();

#ifndef NDEBUG
	GLint logLength = 0;
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 1)
	{
		GLchar *log = static_cast<GLchar *>(malloc(logLength));
		glGetShaderInfoLog(shaderID, logLength, &logLength, log);
		LOG_W("Shader compile log:\n%s", log);
		free(log);
	}
#endif

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glDeleteShader(shaderID);
		LOG_E("Failed to compile shader");
	}

	GL_GET_ERROR();
}

void GLR::Shader::LinkProgram(GLuint programID)
{
	GLint status;

	glLinkProgram(programID);
	GL_GET_ERROR();

#ifndef NDEBUG
	GLint logLength = 0;
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 1)
	{
		GLchar *log = static_cast<GLchar *>(malloc(logLength));
		glGetProgramInfoLog(programID, logLength, &logLength, log);
		LOG_W("Program link log:\n%s", log);
		free(log);
	}
#endif

	glGetProgramiv(programID, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
		LOG_E("Failed to link shader");

	GL_GET_ERROR();
}

void GLR::Shader::ValidateProgram(GLuint programID)
{
	GLint logLength = 0;
	GLint status = 0;

	glValidateProgram(programID);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = static_cast<GLchar *>(malloc(logLength));
		glGetProgramInfoLog(programID, logLength, &logLength, log);
		free(log);
	}

	logLength = 0;
	glGetProgramiv(programID, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE)
		LOG_E("Shader program not valid");

	GL_GET_ERROR();
}

void GLR::Shader::LoadUniforms()
{
	// Get the number of uniforms
	GLint numactiveUniforms = 0;
	glGetProgramiv(m_programID, GL_ACTIVE_UNIFORMS, &numactiveUniforms);

	// Get the maximum name length of the uniforms
	GLint maxUniformNameLength = 0;
	glGetProgramiv(m_programID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
	std::vector<GLchar> uniformNameData(maxUniformNameLength);

	// Loop over all uniforms
	int samplerCount = 0;
	for(int i = 0; i < numactiveUniforms; i++)
	{
		// Get uniform information
		GLint arraySize = 0;
		GLenum type = 0;
		GLsizei actualNameLength = 0;
		glGetActiveUniform(m_programID, i, maxUniformNameLength, &actualNameLength, &arraySize, &type, &uniformNameData[0]);

		// Get uniform name and location
		std::string name(&uniformNameData[0], actualNameLength);
		GLint location = glGetUniformLocation(m_programID, name.c_str());

		// Create and store the uniforms by name
		if(type >= GL_SAMPLER_1D && type <= GL_SAMPLER_2D_SHADOW)
			m_uniforms[name] = InputParameter(name, location, type, samplerCount++);
		else
			m_uniforms[name] = InputParameter(name, location, type);
	}

	GL_GET_ERROR();
}

void GLR::Shader::LoadAttributes()
{
	// Get the number of attributes
	GLint numActiveAttributes = 0;
	glGetProgramiv(m_programID, GL_ACTIVE_ATTRIBUTES, &numActiveAttributes);

	// Get the maximum name length of attributes
	GLint maxAttributeNameLength = 0;
	glGetProgramiv(m_programID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeNameLength);
	std::vector<GLchar> attributeNameData(maxAttributeNameLength);

	// Loop over all attributes
	for(int i = 0; i < numActiveAttributes; i++)
	{
		// Get attribute information
		GLint arraySize = 0;
		GLenum type = 0;
		GLsizei actualNameLength = 0;
		glGetActiveAttrib(m_programID, i, maxAttributeNameLength, &actualNameLength, &arraySize, &type, &attributeNameData[0]);

		// Get attribute name and location
		std::string name(&attributeNameData[0], actualNameLength);
		GLint location = glGetAttribLocation(m_programID, name.c_str());

		// Create and store the attribute by name
		m_attributes[name] = InputParameter(name, location, type);
	}

	GL_GET_ERROR();
}

void GLR::Shader::LoadUniformBlocks()
{
	// Get the number of uniform blocks
	GLint numactiveUniformBlocks = 0;
	glGetProgramiv(m_programID, GL_ACTIVE_UNIFORM_BLOCKS, &numactiveUniformBlocks);

	// Get the maximum name length of the uniform blocks
	GLint maxUniformBlockNameLength = 0;
	glGetProgramiv(m_programID, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxUniformBlockNameLength);
	std::vector<GLchar> uniformBlockNameData(maxUniformBlockNameLength);

	// Loop over all uniform blocks
	unsigned bindingCount = 0;
	for (int i = 0; i < numactiveUniformBlocks; i++)
	{
		// Get the name length of the uniform block
		GLint nameLength;
		glGetActiveUniformBlockiv(m_programID, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLength);

		// Get the name of the uniform block
		std::vector<GLchar> blockName(nameLength);
		glGetActiveUniformBlockName(m_programID, i, nameLength, nullptr, &blockName[0]);
		std::string name(&blockName[0]);

		// Look for the uniform block in the global list
		bool isGlobal = std::binary_search(m_globalUniformBlocks.begin(), m_globalUniformBlocks.end(), name);

		// Look for the global uniform block in the list of uniform blocks
		std::map<std::string, UniformBlock>::iterator it = m_uniformBlocks.end();
		if(isGlobal)
			it = m_uniformBlocks.find(name);

		if(it != m_uniformBlocks.end())
		{
			// If the global block already exists, bind it to the buffer
			glUniformBlockBinding(m_programID, i, it->second.binding);
		}
		else
		{
			// Get the size of the uniform block
			GLint bufferSize;
			glGetActiveUniformBlockiv(m_programID, i, GL_UNIFORM_BLOCK_DATA_SIZE, &bufferSize);

			// Create the buffer
			GLuint buffer;
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_UNIFORM_BUFFER, buffer);
			std::unique_ptr<char> nullMemory(new char[bufferSize]); // Fixes buffer not updating
			glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullMemory.get(), GL_STREAM_DRAW);

			// Bind this programs uniform block to this buffer
			glBindBufferBase(GL_UNIFORM_BUFFER, bindingCount, buffer);

			// Unbind the buffer
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			// Create and store the shader block by name
			m_uniformBlocks[name] = UniformBlock(name, buffer, bindingCount++, bufferSize);
		}
	}

	GL_GET_ERROR();
}
