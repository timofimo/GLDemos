#pragma once

namespace GLR
{
	enum ETextureFlags
	{
		NONE = 0,
		FILTERING_LINEAR = 0x0001,
		FILTERING_MIP_MAP = 0x0002,
		WRAP_MIRRORED_REPEAT = 0x0004,
		WRAP_CLAMP_TO_EDGE = 0x0008,
		WRAP_CLAMP_TO_BORDER = 0x0010,
	};

	class Texture2D : public ManagedItem<Texture2D>
	{
	public:
		Texture2D(const std::string& name, const std::string& file, int flags = NONE);
		Texture2D(const std::string& name, const unsigned char* data, int width, int height, int channels, int flags = NONE);
		Texture2D(const std::string& name, int width, int height, int channels, int msaa, GLenum dataType);
		Texture2D(const std::string& name, int width, int height, int msaa, GLenum depthBufferFormat);
		~Texture2D();

		int GetWidth() const;
		int GetHeight() const;
		int GetChannels() const;
		int GetMSAA() const;
		void GetDimensions(int& width, int& height) const;
		GLuint GetTextureID() const;
		uint64_t GetHandle() const;

	private:
		void InitializeTexture(const unsigned char* data);

		GLuint m_textureID;
		int m_width, m_height, m_channels, m_msaa, m_flags;
		GLenum m_dataType, m_internalFormat, m_format;
		uint64_t m_handle;
	};

}
