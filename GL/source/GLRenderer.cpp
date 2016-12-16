#include "..\include\GLRenderer.h"
#include "Shader.h"
#include <assert.h>
#include "Mesh.h"
#include "Utilities.h"
#include "Texture.h"

namespace GLR
{
	namespace Internal
	{
		struct BlendState
		{
			bool enabled = false;
			GLuint sourceFactor = GL_ONE;
			GLuint destinationFactor = GL_ZERO;

			void SetActive(bool active)
			{
				if (active && !enabled)
				{
					glEnable(GL_BLEND);
					enabled = true;
				}
				else if(!active && enabled)
				{
					glDisable(GL_BLEND);
					enabled = false;
				}
			}

			void SetBlendFunc(GLuint sFactor, GLuint dFactor)
			{
				if (sFactor != sourceFactor || dFactor != destinationFactor)
				{
					glBlendFunc(sFactor, dFactor);
					sourceFactor = sourceFactor;
					destinationFactor = destinationFactor;
				}
			}
		};

		struct DepthState
		{
			bool testEnabled = false;
			bool writeEnabled = true;
			GLuint compareFunction = GL_LESS;

			void SetTestActive(bool active)
			{
				if(active && !testEnabled)
				{
					glEnable(GL_DEPTH_TEST);
					testEnabled = true;
				}
				else if(!active && testEnabled)
				{
					glDisable(GL_DEPTH_TEST);
					testEnabled = false;
				}
			}

			void SetWriteActive(bool active)
			{
				if (active && !writeEnabled)
				{
					glDepthMask(true);
					testEnabled = true;
				}
				else if (!active && writeEnabled)
				{
					glDepthMask(false);
					testEnabled = false;
				}
			}

			void SetCompareFunction(GLuint func)
			{
				if(compareFunction != func)
				{
					glDepthFunc(func);
					compareFunction = func;
				}
			}
		};

		struct StencilState
		{
			bool testEnabled = false;
			bool writeEnabled = true;
			GLuint compareFunction = GL_ALWAYS;
			GLuint compareReference = 0;
			GLuint compareMask = 0xff;
			GLuint testFail = GL_KEEP;
			GLuint testDepthFail = GL_KEEP;
			GLuint testPass = GL_KEEP;

			void SetTestActive(bool active)
			{
				if (active && !testEnabled)
				{
					glEnable(GL_STENCIL_TEST);
					testEnabled = true;
				}
				else if (!active && testEnabled)
				{
					glDisable(GL_STENCIL_TEST);
					testEnabled = false;
				}
			}

			void SetWriteActive(bool active)
			{
				if (active && !writeEnabled)
				{
					glStencilMask(0xff);
					testEnabled = true;
				}
				else if (!active && writeEnabled)
				{
					glStencilMask(0x00);
					testEnabled = false;
				}
			}

			void SetStencilFunction(GLuint func, GLuint ref, GLuint mask)
			{
				if(func != compareFunction || ref != compareReference || mask != compareMask)
				{
					glStencilFunc(func, ref, mask);
					compareFunction = func;
					compareReference = ref;
					compareMask = mask;
				}
			}

			void SetStencilOperation(GLuint fail, GLuint depthFail, GLuint pass)
			{
				if(fail != testFail || depthFail != testDepthFail || pass != testPass)
				{
					glStencilOp(fail, depthFail, pass);
					testFail = fail;
					testDepthFail = depthFail;
					testPass = pass;
				}
			}

		};

		struct RasterizerState
		{
			bool cullFaceEnabled = true;
			GLuint cullFace = GL_BACK;
			GLuint frontFace = GL_CCW;

			void SetActive(bool active)
			{
				if (active && !cullFaceEnabled)
				{
					glEnable(GL_CULL_FACE);
					cullFaceEnabled = true;
				}
				else if (!active && cullFaceEnabled)
				{
					glDisable(GL_CULL_FACE);
					cullFaceEnabled = false;
				}
			}

			void SetCullFace(GLuint face)
			{
				if(face != cullFace)
				{
					glCullFace(face);
					cullFace = face;
				}
			}

			void SetFrontFace(GLuint front)
			{
				if(front != frontFace)
				{
					glFrontFace(front);
					frontFace = front;
				}
			}
		};
		struct RendererState
		{
			// Pipeline states
			BlendState blendState;
			DepthState depthState;
			StencilState stencilState;
			RasterizerState rasterizerState;

			// Bindings
			GLuint boundShaderProgram = 0;
			GLuint boundVertexArray = 0;
			GLint numTextureUnits = 0;
			std::unique_ptr<GLuint> boundTextures = nullptr;
		} rendererState;
	}
}

void GLR::Initialize()
{
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &Internal::rendererState.numTextureUnits);
	Internal::rendererState.boundTextures.reset(new GLuint[Internal::rendererState.numTextureUnits]);
	memset(Internal::rendererState.boundTextures.get(), 0, sizeof(GLuint) * Internal::rendererState.numTextureUnits);
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

void GLR::BindTexture(const Texture2D& texture, unsigned unit)
{
	GLuint textureID = texture.GetTextureID();
	assert(textureID != 0 && "Texture hasn't been initialized");
	assert(textureID < Internal::rendererState.numTextureUnits);

	if (textureID != Internal::rendererState.boundTextures.get()[unit])
	{
		glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0 + unit);
		Internal::rendererState.boundTextures.get()[unit] = textureID;
	}
}

void GLR::SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void GLR::SetBlendState(bool enabled, GLuint sFactor, GLuint dFactor)
{
	Internal::rendererState.blendState.SetActive(enabled);
	Internal::rendererState.blendState.SetBlendFunc(sFactor, dFactor);
}

void GLR::SetDepthState(bool depthTest, bool depthWrite, GLuint depthCompareFunction)
{
	Internal::rendererState.depthState.SetTestActive(depthTest);
	Internal::rendererState.depthState.SetWriteActive(depthWrite);
	Internal::rendererState.depthState.SetCompareFunction(depthCompareFunction);
}

void GLR::SetStencilState(bool stencilTest, bool stencilWrite, GLuint compareFunc, GLuint compareRef, GLuint compareMask, GLuint testFail, GLuint testDepthFail, GLuint testPass)
{
	Internal::rendererState.stencilState.SetTestActive(stencilTest);
	Internal::rendererState.stencilState.SetWriteActive(stencilWrite);
	Internal::rendererState.stencilState.SetStencilFunction(compareFunc, compareRef, compareMask);
	Internal::rendererState.stencilState.SetStencilOperation(testFail, testDepthFail, testPass);
}

void GLR::SetRasterizationState(bool faceCulling, GLuint cullFace, GLuint windingOrder)
{
	Internal::rendererState.rasterizerState.SetActive(faceCulling);
	Internal::rendererState.rasterizerState.SetCullFace(cullFace);
	Internal::rendererState.rasterizerState.SetFrontFace(windingOrder);
}
