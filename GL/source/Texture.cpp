#include "..\include\Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push, 0)
#include <stb/stb_image.h>
#pragma warning(pop)

GLR::Texture2D::Texture2D(const std::string& name, const std::string& file, int flags) : ManagedItem<GLR::Texture2D>(name, this)
{
	m_flags = flags;
	unsigned char* data = stbi_load(file.c_str(), &m_width, &m_height, &m_channels, 0);
	if (data != nullptr)
	{
		InitializeTexture(data);
		stbi_image_free(data);
	}
	else
	{
		LOG_E("Failed to load texture");
	}
}

GLR::Texture2D::Texture2D(const std::string& name, const unsigned char* data, int width, int height, int channels, int flags) : ManagedItem<GLR::Texture2D>(name, this)
{
	m_flags = flags;
	m_width = width;
	m_height = height;
	m_channels = channels;
	InitializeTexture(data);
}

GLR::Texture2D::~Texture2D()
{
	if(m_textureID != 0)
	{
		glDeleteTextures(1, &m_textureID);
		m_textureID = 0;
	}
}

int GLR::Texture2D::GetWidth() const
{
	return m_width;
}

int GLR::Texture2D::GetHeight() const
{
	return m_height;
}

int GLR::Texture2D::GetChannels() const
{
	return m_channels;
}

void GLR::Texture2D::GetDimensions(int& width, int& height) const
{
	width = m_width;
	height = m_height;
}

GLuint GLR::Texture2D::GetTextureID() const
{
	return m_textureID;
}

void GLR::Texture2D::InitializeTexture(const unsigned char* data)
{
	glGenTextures(1, &m_textureID);
	GL_GET_ERROR();

	glBindTexture(GL_TEXTURE_2D, m_textureID);
	GL_GET_ERROR();

	if (m_flags & WRAP_MIRRORED_REPEAT)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}
	else if (m_flags & WRAP_CLAMP_TO_EDGE)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else if (m_flags & WRAP_CLAMP_TO_BORDER)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	GL_GET_ERROR();

	if (m_flags & FILTERING_LINEAR)
	{
		if (m_flags & FILTERING_MIP_MAP)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}
	else if (m_flags & FILTERING_MIP_MAP)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	GL_GET_ERROR();

	unsigned format;
	switch (m_channels)
	{
	case 1:
		format = GL_RED;
		break;
	case 2:
		format = GL_RG;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		LOG_E("Unknown amount of channels in texture");
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_width, 0, format, GL_UNSIGNED_BYTE, data);
	GL_GET_ERROR();

	if (m_flags & FILTERING_MIP_MAP)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
		GL_GET_ERROR();
	}
}
