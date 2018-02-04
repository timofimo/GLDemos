#pragma once

class Input
{
public:
	static void KeyCallback(int key, int action);

	static void Update();

	static bool GetKeyDown(int key);

	static bool GetKeyUp(int key);

	static bool GetKey(int key);

private:
	enum class EKeyState
	{
		UP, PRESSED, DOWN, RELEASED,
	} static m_keyStates[GLFW_KEY_LAST];
};