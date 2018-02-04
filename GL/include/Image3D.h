#pragma once

namespace GLR
{
	class Image3D : public ManagedItem<Image3D>
	{
	public:
		Image3D(const std::string& name, int width, int height, int depth, GLenum internalFormat);
		~Image3D();

		void Clear() const;
		void Clear(const void* value, GLenum type) const;

		int GetWidth() const;
		int GetHeight() const;
		int GetDepth() const;
		void GetDimensions(int& width, int& height, int& depth) const;
		GLuint GetImageID() const;
		GLenum GetFormat() const;
		GLenum GetInternalFormat() const;

	private:
		GLenum GetFormat(GLenum internalFormat);

		GLuint m_imageID;
		int m_width, m_height, m_depth;
		GLenum m_format, m_internalFormat;
	};
}