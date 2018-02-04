#include "PCH.h"
#include "Input.h"

Input::EKeyState Input::m_keyStates[GLFW_KEY_LAST];

void Input::KeyCallback(int key, int action)
{
	switch (action)
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

void Input::Update()
{
	for (unsigned i = 0; i < GLFW_KEY_LAST; i++)
	{
		switch (m_keyStates[i])
		{
		case EKeyState::PRESSED: m_keyStates[i] = EKeyState::DOWN; break;
		case EKeyState::RELEASED: m_keyStates[i] = EKeyState::UP; break;
		default: break;
		}
	}
}

bool Input::GetKeyDown(int key)
{
	return m_keyStates[key] == EKeyState::PRESSED;
}

bool Input::GetKeyUp(int key)
{
	return m_keyStates[key] == EKeyState::RELEASED;
}

bool Input::GetKey(int key)
{
	return m_keyStates[key] == EKeyState::DOWN || m_keyStates[key] == EKeyState::PRESSED;
}