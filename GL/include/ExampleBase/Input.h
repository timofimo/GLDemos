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
			m_keyStates[key] = EKeyState::Pressed;
			return;
		case GLFW_RELEASE:
			m_keyStates[key] = EKeyState::Released;
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
			case EKeyState::Pressed: m_keyStates[i] = EKeyState::Down; break;
			case EKeyState::Released: m_keyStates[i] = EKeyState::Up; break;
			default: break;
			}
		}
	}

	static bool GetKeyDown(int key)
	{
		return m_keyStates[key] == EKeyState::Pressed;
	}

	static bool GetKeyUp(int key)
	{
		return m_keyStates[key] == EKeyState::Released;
	}

	static bool GetKey(int key)
	{
		return m_keyStates[key] == EKeyState::Down || m_keyStates[key] == EKeyState::Pressed;
	}

private:
	enum class EKeyState
	{
		Up, Pressed, Down, Released,
	} static m_keyStates[GLFW_KEY_LAST];
};

Input::EKeyState Input::m_keyStates[GLFW_KEY_LAST];