#pragma once
#include "Texture.h"

namespace GLR
{
	struct ColorAttachmentDescription
	{
		unsigned channels;
		GLenum dataType;
	};

	class Framebuffer : public ManagedItem<Framebuffer>
	{
	public:
		Framebuffer(const std::string& name, int width, int height, int msaa, const std::vector<ColorAttachmentDescription>& colorAttachmentDescriptions, GLenum depthStencilType = GL_FALSE);
		~Framebuffer();

		GLuint GetFramebufferID() const;
		int GetMSAA() const;

	private:
		GLuint m_framebufferID;
		std::vector<Texture2D> m_textures;
	};

}
