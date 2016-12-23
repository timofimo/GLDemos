#include "..\include\GLRenderer.h"
#include "Shader.h"
#include <assert.h>
#include "Mesh.h"
#include "Utilities.h"
#include "Texture.h"
#include "Framebuffer.h"
#include <queue>

namespace GLR
{
	namespace Internal
	{
		struct ViewportState
		{
			int x = -1;
			int y = -1;
			int width = 2;
			int height = 2;

			void SetViewport(int _x, int _y, int _width, int _height)
			{
				if(_x != x || _y != y || _width != width || _height != height)
				{
					glViewport(_x, _y, _width, _height);
					x = _x;
					y = _y;
					width = _width;
					height = _height;
				}
			}
		};
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
			bool cullFaceEnabled = false;
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
			ViewportState viewportState;
			BlendState blendState;
			DepthState depthState;
			StencilState stencilState;
			RasterizerState rasterizerState;

			// Bindings
			GLuint boundShaderProgram = 0;
			GLuint boundVertexArray = 0;
			std::unique_ptr<GLuint> boundTextures = nullptr;
			GLuint boundFramebuffer = 0;

			// System limits
			GLint numTextureUnits = 0;
			GLint maxUniformBlockSize = 0;
			GLint maxShaderStorageBlockSize = 0;
		} rendererState;

		struct DrawCommandBuffer
		{
			unsigned bufferID;
			unsigned count;

			DrawCommandBuffer() : bufferID(0), count(0){}
			DrawCommandBuffer(const void* data, unsigned size, unsigned count) : count(count)
			{
				glGenBuffers(1, &bufferID);
				GL_GET_ERROR();

				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, bufferID);
				GL_GET_ERROR();
				glBufferData(GL_DRAW_INDIRECT_BUFFER, size, data, GL_STATIC_DRAW);
				GL_GET_ERROR();

				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
			}
			DrawCommandBuffer(DrawCommandBuffer&& other)
			{
				bufferID = std::move(other.bufferID);
				count = std::move(other.count);
				other.bufferID = 0;
			}
			~DrawCommandBuffer()
			{
				if(bufferID != 0)
				{
					glDeleteBuffers(1, &bufferID);
					bufferID = 0;
				}
			}

			void operator=(DrawCommandBuffer&& rhs)
			{
				bufferID = std::move(rhs.bufferID);
				count = std::move(rhs.count);
				rhs.bufferID = 0;
			}
		};
		std::vector<DrawCommandBuffer> m_drawCommandBuffers;
		std::queue<unsigned> m_availableDrawCommandSlots;
	}
}

void GLR::Initialize()
{
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &Internal::rendererState.numTextureUnits);
	Internal::rendererState.boundTextures.reset(new GLuint[Internal::rendererState.numTextureUnits]);
	memset(Internal::rendererState.boundTextures.get(), 0, sizeof(GLuint) * Internal::rendererState.numTextureUnits);

	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &Internal::rendererState.maxUniformBlockSize);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &Internal::rendererState.maxShaderStorageBlockSize);
}

void GLR::Clear(GLbitfield mask)
{
	glClear(mask);
	GL_GET_ERROR();
}

void GLR::DrawIndexed(unsigned count, unsigned offset)
{
	offset *= sizeof(unsigned);
	uint64_t tempOffset = offset;
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, reinterpret_cast<void*>(tempOffset));
	GL_GET_ERROR();
}

void GLR::DrawIndexedIndirect(unsigned bufferIndex)
{
	GL_GET_ERROR();
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, Internal::m_drawCommandBuffers[bufferIndex].bufferID);
	GL_GET_ERROR();

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, Internal::m_drawCommandBuffers[bufferIndex].count, 0);
	GL_GET_ERROR();

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

unsigned GLR::CreateDrawCommandBuffer(const std::vector<DrawElementsIndirectCommand>& drawCommands)
{
	if (Internal::m_availableDrawCommandSlots.empty())
	{
		Internal::m_drawCommandBuffers.push_back(std::move(Internal::DrawCommandBuffer(drawCommands.data(), sizeof(GLR::DrawElementsIndirectCommand) * unsigned(drawCommands.size()), unsigned(drawCommands.size()))));
		return unsigned(Internal::m_drawCommandBuffers.size()) - 1;
	}

	unsigned i = Internal::m_availableDrawCommandSlots.front();
	Internal::m_availableDrawCommandSlots.pop();
	Internal::m_drawCommandBuffers[i] = std::move(Internal::DrawCommandBuffer(drawCommands.data(), sizeof(GLR::DrawElementsIndirectCommand) * unsigned(drawCommands.size()), unsigned(drawCommands.size())));
	return i;
}

void GLR::DestroyDrawCommandBuffer(unsigned index)
{
	Internal::m_drawCommandBuffers[index] = Internal::DrawCommandBuffer();
	Internal::m_availableDrawCommandSlots.push(index);
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
	assert(textureID < unsigned(Internal::rendererState.numTextureUnits));

	if (textureID != Internal::rendererState.boundTextures.get()[unit])
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		GL_GET_ERROR();
		glBindTexture(GL_TEXTURE_2D, textureID);
		GL_GET_ERROR();
		Internal::rendererState.boundTextures.get()[unit] = textureID;
	}
}

void GLR::BindFramebuffer(const Framebuffer& framebuffer)
{
	GLuint framebufferID = framebuffer.GetFramebufferID();
	assert(framebufferID != 0 && "Framebuffer hasn't been initialized");

	if(framebufferID != Internal::rendererState.boundFramebuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.GetFramebufferID());
		Internal::rendererState.boundFramebuffer = framebufferID;
	}
}

void GLR::UnbindFramebuffer()
{
	if (Internal::rendererState.boundFramebuffer != 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Internal::rendererState.boundFramebuffer = 0;
	}
}

void GLR::UnbindShader()
{
	if(Internal::rendererState.boundShaderProgram != 0)
	{
		glUseProgram(0);
		Internal::rendererState.boundShaderProgram = 0;
	}
}

void GLR::SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void GLR::SetViewport(int x, int y, int width, int height)
{
	Internal::rendererState.viewportState.SetViewport(x, y, width, height);
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

GLuint GLR::GetMaxTextureUnit()
{
	return Internal::rendererState.numTextureUnits;
}
