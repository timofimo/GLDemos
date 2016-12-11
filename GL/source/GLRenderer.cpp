#include "..\include\GLRenderer.h"
#include "Shader.h"
#include <assert.h>
#include "Mesh.h"
#include "Utilities.h"

namespace GLR
{
	namespace Internal
	{
		struct RendererState
		{
			GLuint boundShaderProgram = 0;
			GLuint boundVertexArray = 0;
		} rendererState;
	}
}

void GLR::Clear(GLbitfield mask)
{
	glClear(mask);
	GL_GET_ERROR();
}

void GLR::DrawIndexed(unsigned count)
{
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	GL_GET_ERROR();
}

void GLR::BindMesh(const Mesh& mesh)
{
	GLuint vao = mesh.GetVertexArray();
	assert(vao != 0 && "Mesh hasn't been initialized");

	if(vao != Internal::rendererState.boundVertexArray)
	{
		glBindVertexArray(vao);
		Internal::rendererState.boundVertexArray = vao;
	}
}

void GLR::BindShader(const Shader& shader)
{
	GLuint program = shader.GetProgram();
	assert(program != 0 && "Shader hasn't been initialized");

	if (program != Internal::rendererState.boundShaderProgram)
	{
		glUseProgram(shader.GetProgram());
		Internal::rendererState.boundShaderProgram = program;
	}
}

void GLR::SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}
