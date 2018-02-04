#pragma once

namespace GLR
{
	class IWindow
	{
	public:
		virtual ~IWindow()
		{
		}

		/**
		 * \brief Initializes the window
		 * \param width The resolution of the window horizontally
		 * \param height The resolution of the window vertically
		 * \param title The name displayed in the title bar
		 * \param borderless If the window should be borderless
		 */
		virtual void Initialize(unsigned width, unsigned height, const char* title, bool borderless) = 0;

		/**
		 * \brief Destroys the window
		 */
		virtual void Destroy() = 0;

		/**
		 * \brief Returns the resolution of the window
		 * \param outWidth Return parameter for the horizontal resolution
		 * \param outHeight Return parameter for the vertical resolution
		 */
		virtual void GetWindowRes(unsigned& outWidth, unsigned& outHeight) = 0;

		/**
		 * \brief Closes the window.
		 */
		virtual void Close() = 0;

		/**
		 * \brief Returns true if the window should close
		 */
		virtual bool ShouldClose() = 0;

		/**
		 * \brief Swaps the back buffer
		 */
		virtual void SwapBuffers() = 0;

	};

	class GLFWWindow : public IWindow
	{
	public:
		GLFWWindow();
		GLFWWindow(unsigned width, unsigned height, const char* title, bool borderless);
		~GLFWWindow();

		void Initialize(unsigned width, unsigned height, const char* title, bool borderless) override;
		void Destroy() override;
		void GetWindowRes(unsigned& outWidth, unsigned& outHeight) override;
		void Close() override;
		bool ShouldClose() override;
		void SwapBuffers() override;
		GLFWwindow* GetHandle() const;

		static void SetInputCallback(void(*function)(int, int));
	private:
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void ErrorCallback(int error, const char* description);

		GLFWwindow* m_window;
		unsigned m_width, m_height;
		static void(*m_inputCallback)(int, int);
	};
}
