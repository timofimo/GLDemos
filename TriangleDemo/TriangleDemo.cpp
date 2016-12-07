#include "GL.h"
#include "Window.h"

int main()
{
	GL::GLFWWindow window(640, 480, "TriangleDemo", true);

	while(!window.ShouldClose())
	{
		window.SwapBuffers();
	}

	return 0;
}
