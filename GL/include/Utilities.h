#pragma once

namespace GLR
{
#define LOG_E(macro_format, ...)									\
{																	\
	char buffer[1024];												\
	sprintf_s(buffer, macro_format, ##__VA_ARGS__);					\
    assert(false);								\
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

	template <class T>
	class ManagedItem
	{
	private:
		static std::map<std::string, T*> m_items;

	protected:
		std::string m_name;

	public:
		ManagedItem(const std::string& name, T* ptr)
		{
			auto it = m_items.find(name);
			if (it == m_items.end())
			{
				m_items[name] = ptr;
				m_name = name;
				return;
			}

			LOG_E("Item with that name already exists");
		}

		virtual ~ManagedItem()
		{
			m_items.erase(m_name);
		}
		
		static T* GetItem(const std::string& name)
		{
			auto it = m_items.find(name);
			if(it != m_items.end())
				return it->second;

			return nullptr;
		}
	};
	template<class T>
	std::map<std::string, T*> ManagedItem<T>::m_items;

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
