#include "..\include\Framebuffer.h"
#include <string>


GLR::Framebuffer::Framebuffer(const std::string& name, int width, int height, const std::vector<ColorAttachmentDescription>& colorAttachmentDescriptions, GLenum depthStencilType) : ManagedItem<GLR::Framebuffer>(name, this)
{
	glGenFramebuffers(1, &m_framebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);
	GL_GET_ERROR();

	std::vector<GLenum> drawBuffers;
	m_textures.reserve(colorAttachmentDescriptions.size() + (depthStencilType == GL_FALSE ? 0 : 1));
	for(unsigned i = 0; i < unsigned(colorAttachmentDescriptions.size()); i++)
	{
		std::string textureName = name + "_" + std::to_string(i);
		m_textures.emplace_back(textureName, width, height, colorAttachmentDescriptions[i].channels, colorAttachmentDescriptions[i].dataType);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures.back().GetTextureID(), 0);
		drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
	}
	GL_GET_ERROR();

	glDrawBuffers(unsigned(drawBuffers.size()), drawBuffers.data());
	GL_GET_ERROR();

	if(depthStencilType != GL_FALSE)
	{
		std::string textureName = name + "_depth";
		m_textures.emplace_back(textureName, width, height, depthStencilType);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textures.back().GetTextureID(), 0);
		GL_GET_ERROR();
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG_E("Failed to create framebuffer");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GL_GET_ERROR();
}

GLR::Framebuffer::~Framebuffer()
{
	if(m_framebufferID != 0)
	{
		glDeleteFramebuffers(1, &m_framebufferID);
		m_framebufferID = 0;
	}
}

GLuint GLR::Framebuffer::GetFramebufferID() const
{
	return m_framebufferID;
}
