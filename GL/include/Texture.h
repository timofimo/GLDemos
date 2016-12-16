#pragma once
#include "Utilities.h"

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
		~Texture2D();

		int GetWidth() const;
		int GetHeight() const;
		int GetChannels() const;
		void GetDimensions(int& width, int& height) const;
		GLuint GetTextureID() const;

	private:
		void InitializeTexture(const unsigned char* data);

		GLuint m_textureID;
		int m_width, m_height, m_channels, m_flags;
	};

}
