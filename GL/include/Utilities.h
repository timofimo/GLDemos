#pragma once
#include <fstream>
#include <glad/glad.h>

namespace GLR
{
#define LOG_E(macro_format, ...)									\
{																	\
	char buffer[1024];												\
	sprintf_s(buffer, macro_format, ##__VA_ARGS__);					\
    throw std::runtime_error(buffer);								\
}

	inline void ReadFile(const char* file, char* (&data), unsigned& size)
	{
		std::ifstream stream(file, std::ios::binary | std::ios::ate);
		if(stream.is_open())
		{
			size = unsigned(stream.tellg());
			data = new char[size + 1];
			stream.seekg(0, std::ios::beg);
			stream.read(data, size);
			stream.close();

			data[size] = '\0'; // Null terminated
		}
		else
		{
			LOG_E("Couldn't read file.");
		}
	}


#ifndef NDEBUG

	// Convert a GLenum to a readable c-string
	inline const char* GLErrorString(GLenum error)
	{
		switch (error)
		{
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";

		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";

		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";

		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";

		default:
			return "UNKNOWN";
		}
	}

#define LOG_W(macro_format, ...)									\
{																	\
	printf(macro_format, ##__VA_ARGS__);							\
    const size_t macro_len = strlen(macro_format);					\
    if(macro_len > 0 && macro_format[macro_len - 1] != '\n')		\
	{																\
        printf_s("\n");												\
    }																\
}

#define GL_GET_ERROR()                                              \
{                                                                   \
    GLenum error;                                                   \
    bool err = false;                                               \
    while ( (error = glGetError()) != GL_NO_ERROR)                  \
    {                                                               \
        err = true;                                                 \
        LOG_E( "OpenGL ERROR: %s\nCHECK POINT: %s (line %d)\n",		\
        GLR::GLErrorString(error), __FILE__, __LINE__ );			\
    }                                                               \
    assert(!err);                                                   \
}

#define GL_CLEAR_ERROR()                                            \
{                                                                   \
    GLenum error;                                                   \
    while ( (error = glGetError()) != GL_NO_ERROR);                 \
}

#else
#define GL_CLEAR_ERROR()
#define GL_GET_ERROR()
#define LOG_W(...) { }
#endif
}
