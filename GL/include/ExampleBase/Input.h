#pragma once
#include <GLFW/glfw3.h>

class Input
{
public:
	static void KeyCallback(int key, int action)
	{
		switch(action)
		{
		case GLFW_PRESS:
			m_keyStates[key] = EKeyState::PRESSED;
			return;
		case GLFW_RELEASE:
			m_keyStates[key] = EKeyState::RELEASED;
			return;
		default:
			return;
		}
	}

	static void Update()
	{
		for(unsigned i = 0; i < GLFW_KEY_LAST; i++)
		{
			switch (m_keyStates[i])
			{
			case EKeyState::PRESSED: m_keyStates[i] = EKeyState::DOWN; break;
			case EKeyState::RELEASED: m_keyStates[i] = EKeyState::UP; break;
			default: break;
			}
		}
	}

	static bool GetKeyDown(int key)
	{
		return m_keyStates[key] == EKeyState::PRESSED;
	}

	static bool GetKeyUp(int key)
	{
		return m_keyStates[key] == EKeyState::RELEASED;
	}

	static bool GetKey(int key)
	{
		return m_keyStates[key] == EKeyState::DOWN || m_keyStates[key] == EKeyState::PRESSED;
	}

private:
	enum class EKeyState
	{
		UP, PRESSED, DOWN, RELEASED,
	} static m_keyStates[GLFW_KEY_LAST];
};

Input::EKeyState Input::m_keyStates[GLFW_KEY_LAST];