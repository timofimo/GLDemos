#include <PCH.h>
#include <Image3D.h>
#include "GLRenderer.h"

#define FORMAT_CASE(Comp, Base, Type, Bits)	case GL_##Comp##Bits: return GL_##Base##Type;

#define FORMAT_BITS(Comp, Base)			\
	FORMAT_CASE(Comp, Base, , 8)			\
	FORMAT_CASE(Comp, Base, , 16)			\
	FORMAT_CASE(Comp, Base, , 8_SNORM)	\
	FORMAT_CASE(Comp, Base, , 16_SNORM)	\
	FORMAT_CASE(Comp, Base, _INTEGER, 8I)			\
	FORMAT_CASE(Comp, Base, _INTEGER, 16I)		\
	FORMAT_CASE(Comp, Base, _INTEGER, 32I)		\
	FORMAT_CASE(Comp, Base, _INTEGER, 8UI)		\
	FORMAT_CASE(Comp, Base, _INTEGER, 16UI)		\
	FORMAT_CASE(Comp, Base, _INTEGER, 32UI)		\
	FORMAT_CASE(Comp, Base, _INTEGER, 16F)		\
	FORMAT_CASE(Comp, Base, _INTEGER, 32F)

#define FORMAT_SWITCH(InternalFormat)		\
	switch(InternalFormat){					\
	FORMAT_BITS(R, RED)						\
	FORMAT_BITS(RG, RG)						\
	FORMAT_BITS(RGB, RGB)					\
	FORMAT_BITS(RGBA, RGBA)					\
	}

GLR::Image3D::Image3D(const std::string& name, int width, int height, int depth, GLenum internalFormat) : ManagedItem<Image3D>(name, this)
{
	m_width = width;
	m_height = height;
	m_depth = depth;
	m_format = GetFormat(internalFormat);
	m_internalFormat = internalFormat;

	glGenTextures(1, &m_imageID);
	GL_GET_ERROR();

	BindTexture(*this, GetMaxTextureUnit() - 1); 
	glTexStorage3D(GL_TEXTURE_3D, 1, internalFormat, width, height, depth);
	GL_GET_ERROR();
}

GLR::Image3D::~Image3D()
{
	if (m_imageID != 0)
	{
		glDeleteTextures(1, &m_imageID);
		m_imageID = 0;
	}
}

void GLR::Image3D::Clear() const
{
	glClearTexSubImage(m_imageID, 0, 0, 0, 0, m_width, m_height, m_depth, m_format, GL_UNSIGNED_INT, nullptr);
	GL_GET_ERROR();
}

void GLR::Image3D::Clear(const void* value, GLenum type) const
{
	glClearTexSubImage(m_imageID, 0, 0, 0, 0, m_width, m_height, m_depth, m_format, type, value);
	GL_GET_ERROR();
}

int GLR::Image3D::GetWidth() const
{
	return m_width;
}

int GLR::Image3D::GetHeight() const
{
	return m_height;
}

int GLR::Image3D::GetDepth() const
{
	return m_depth;
}

void GLR::Image3D::GetDimensions(int& width, int& height, int& depth) const
{
	width = m_width;
	height = m_height;
	depth = m_depth;
}

GLuint GLR::Image3D::GetImageID() const
{
	return m_imageID;
}

GLenum GLR::Image3D::GetFormat() const
{
	return m_format;
}

GLenum GLR::Image3D::GetInternalFormat() const
{
	return m_internalFormat;
}

GLenum GLR::Image3D::GetFormat(GLenum internalFormat)
{
	FORMAT_SWITCH(internalFormat)
	LOG_E("Unknown combination internal format.");
	return 0;
}
