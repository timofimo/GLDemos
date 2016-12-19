#include "..\include\Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push, 0)
#include <stb/stb_image.h>
#include <memory>
#include "GLRenderer.h"
#pragma warning(pop)

GLR::Texture2D::Texture2D(const std::string& name, const std::string& file, int flags) : ManagedItem<GLR::Texture2D>(name, this)
{
	m_flags = flags;
	unsigned char* data = stbi_load(file.c_str(), &m_width, &m_height, &m_channels, 0);
	m_dataType = GL_UNSIGNED_BYTE;
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
	m_dataType = GL_UNSIGNED_BYTE;
	InitializeTexture(data);
}

GLR::Texture2D::Texture2D(const std::string& name, int width, int height, int channels, GLenum dataType) : ManagedItem<GLR::Texture2D>(name, this)
{
	m_flags = NONE;
	m_width = width;
	m_height = height;
	m_channels = channels;
	m_dataType = dataType;

	InitializeTexture(nullptr);
}

GLR::Texture2D::Texture2D(const std::string& name, int width, int height, GLenum depthBufferFormat) : ManagedItem<GLR::Texture2D>(name, this)
{
	m_flags = NONE;
	m_width = width;
	m_height = height;
	m_channels = 1;

	m_internalFormat = depthBufferFormat;
	switch (depthBufferFormat)
	{
	case GL_DEPTH24_STENCIL8:
		m_format = GL_DEPTH_STENCIL;
		m_dataType = GL_UNSIGNED_INT_24_8;
		break;
	case GL_DEPTH32F_STENCIL8:
		m_format = GL_DEPTH_STENCIL;
		m_dataType = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
		break;
	case GL_DEPTH_COMPONENT16:
		m_format = GL_DEPTH_COMPONENT;
		m_dataType = GL_UNSIGNED_SHORT;
		break;
	case GL_DEPTH_COMPONENT24:
		m_format = GL_DEPTH_COMPONENT;
		m_dataType = GL_UNSIGNED_INT;
		break;
	case GL_DEPTH_COMPONENT32:
		m_format = GL_DEPTH_COMPONENT;
		m_dataType = GL_UNSIGNED_INT;
		break;
	case GL_DEPTH_COMPONENT32F:
		m_format = GL_DEPTH_COMPONENT;
		m_dataType = GL_FLOAT;
		break;
	}

	InitializeTexture(nullptr);
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

	BindTexture(*this, GetMaxTextureUnit() - 1);

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

	if (m_format != GL_DEPTH_STENCIL && m_format != GL_DEPTH_COMPONENT)
	{
		switch (m_channels)
		{
		case 1:
			m_format = GL_RED;
			switch (m_dataType)
			{
			case GL_BYTE:
				m_internalFormat = GL_R8;
				break;
			case GL_UNSIGNED_BYTE:
				m_internalFormat = GL_R8_SNORM;
				break;
			case GL_FLOAT:
				m_internalFormat = GL_R32F;
				break;
			case GL_INT:
				m_internalFormat = GL_R32I;
				break;
			case GL_UNSIGNED_INT:
				m_internalFormat = GL_R32UI;
				break;
			case GL_SHORT:
				m_internalFormat = GL_R16I;
				break;
			case GL_UNSIGNED_SHORT:
				m_internalFormat = GL_R16UI;
				break;
			default:
				LOG_E("Unknown combination of channels and data type when creating the texture");
			}
			break;
		case 2:
			m_format = GL_RG;
			switch (m_dataType)
			{
			case GL_BYTE:
				m_internalFormat = GL_RG8;
				break;
			case GL_UNSIGNED_BYTE:
				m_internalFormat = GL_RG8_SNORM;
				break;
			case GL_FLOAT:
				m_internalFormat = GL_RG32F;
				break;
			case GL_INT:
				m_internalFormat = GL_RG32I;
				break;
			case GL_UNSIGNED_INT:
				m_internalFormat = GL_RG32UI;
				break;
			case GL_SHORT:
				m_internalFormat = GL_RG16I;
				break;
			case GL_UNSIGNED_SHORT:
				m_internalFormat = GL_RG16UI;
				break;
			default:
				LOG_E("Unknown combination of channels and data type when creating the texture");
			}
			break;
		case 3:
			m_format = GL_RGB;
			switch (m_dataType)
			{
			case GL_BYTE:
				m_internalFormat = GL_RGB8;
				break;
			case GL_UNSIGNED_BYTE:
				m_internalFormat = GL_RGB8_SNORM;
				break;
			case GL_FLOAT:
				m_internalFormat = GL_RGB32F;
				break;
			case GL_INT:
				m_internalFormat = GL_RGB32I;
				break;
			case GL_UNSIGNED_INT:
				m_internalFormat = GL_RGB32UI;
				break;
			case GL_SHORT:
				m_internalFormat = GL_RGB16I;
				break;
			case GL_UNSIGNED_SHORT:
				m_internalFormat = GL_RGB16UI;
				break;
			default:
				LOG_E("Unknown combination of channels and data type when creating the texture");
			}
			break;
		case 4:
			m_format = GL_RGBA;
			switch (m_dataType)
			{
			case GL_BYTE:
				m_internalFormat = GL_RGBA8_SNORM;
				break;
			case GL_UNSIGNED_BYTE:
				m_internalFormat = GL_RGBA8;
				break;
			case GL_FLOAT:
				m_internalFormat = GL_RGBA32F;
				break;
			case GL_INT:
				m_internalFormat = GL_RGBA32I;
				break;
			case GL_UNSIGNED_INT:
				m_internalFormat = GL_RGBA32UI;
				break;
			case GL_SHORT:
				m_internalFormat = GL_RGBA16I;
				break;
			case GL_UNSIGNED_SHORT:
				m_internalFormat = GL_RGBA16UI;
				break;
			default:
				LOG_E("Unknown combination of channels and data type when creating the texture");
			}
			break;
		default:
			LOG_E("Unknown amount of channels in texture");
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_format, m_dataType, data);
	GL_GET_ERROR();

	if (m_flags & FILTERING_MIP_MAP)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
		GL_GET_ERROR();
	}
}
