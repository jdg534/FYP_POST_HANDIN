#include "UserControledCamera.h"

#include <Windows.h> // for GetAsyncKeyState(), not platform independent

UserControledCamera::UserControledCamera()
{
	m_moveSpeed = 0.75f;
}

UserControledCamera::~UserControledCamera()
{

}

void UserControledCamera::shutdown()
{
	// nothing on the heap
}

void UserControledCamera::update(float dt)
{
	float moveBy = dt * m_moveSpeed;

	if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000)
	{
		m_eyePosition.x -= moveBy;
	}
	else if (GetAsyncKeyState(VK_NUMPAD3) & 0x8000)
	{
		m_eyePosition.x += moveBy;
	}

	// 5 and 2 for Z
	if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000)
	{
		m_eyePosition.z += moveBy;
	}
	else if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000)
	{
		m_eyePosition.z -= moveBy;
	}

	// 4 & 6 for Y
	if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000)
	{
		m_eyePosition.y += moveBy;
	}
	else if (GetAsyncKeyState(VK_NUMPAD6) & 0x8000)
	{
		m_eyePosition.y -= moveBy;
	}

	// now the look at vec, WASD + Q&E for Y axis
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_lookAtPosition.x -= moveBy;
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		m_lookAtPosition.x += moveBy;
	}

	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_lookAtPosition.z += moveBy;
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		m_lookAtPosition.z -= moveBy;
	}

	if (GetAsyncKeyState('Q') & 0x8000)
	{
		m_lookAtPosition.y += moveBy;
	}
	else if (GetAsyncKeyState('E') & 0x8000)
	{
		m_lookAtPosition.y -= moveBy;
	}

}
