#include "FlyingCamera.h"

#include <Windows.h>
#include <DirectXMath.h>

void FlyingCamera::shutdown()
{
	// nothing on the heap :)
}

void FlyingCamera::update(float dtSec)
{
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		m_moveSpeed += 0.00001f;
	}
	else if (GetAsyncKeyState('E') & 0x8000)
	{
		m_moveSpeed -= 0.00001f;
	}


	float moveSpeed = m_moveSpeed * dtSec;

	float turnSpeed = 0.01f;
	turnSpeed *= dtSec;
	// alter yaw & pitch with arrow keys (for now!!!)

	// Yaw for rotate around Y axis (turn L & R)

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_yaw -= turnSpeed;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_yaw += turnSpeed;
	}

	// pitch for x axis (looking up & down)
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m_pitch -= turnSpeed;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		m_pitch += turnSpeed;
	}

	/* (old code)
	XMVECTOR z = XMLoadFloat3(&m_at);
	XMVECTOR y = XMLoadFloat3(&m_up);
	XMVECTOR x = XMVector3Cross(y,z);

	z = XMVector3Normalize(z);
	y = XMVector3Normalize(y);
	x = XMVector3Normalize(x);

	XMFLOAT3 right;
	XMStoreFloat3(&right, x);
	*/

	// based off http://www.braynzarsoft.net/viewtutorial/q16390-18-first-person-camera

	using namespace DirectX;

	XMMATRIX rotM = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f);

	XMVECTOR targetVec = XMVector3TransformCoord(XMLoadFloat3(&m_defaultForward), rotM);
	targetVec = XMVector3Normalize(targetVec);
	// rember the lookAt vec = eyePosition + targetVec
	XMStoreFloat3(&m_lookAtDir, targetVec);

	XMMATRIX tmpYRotMat = XMMatrixRotationY(m_yaw);

	/* transform:
	XMFLOAT3 m_right;
	XMFLOAT3 m_forward;
	XMFLOAT3 m_upVecForFlyCam;
	*/

	XMVECTOR dr, r,
		df, f, upTmp, upRes;
	dr = XMLoadFloat3(&m_defaultRight);
	df = XMLoadFloat3(&m_defaultForward);
	upTmp = XMLoadFloat3(&m_upVecForFlyCam);

	r = XMVector3TransformNormal(dr, tmpYRotMat);
	f = XMVector3TransformNormal(df, tmpYRotMat);
	upRes = XMVector3TransformNormal(upTmp, tmpYRotMat);

	XMStoreFloat3(&m_right, r);
	XMStoreFloat3(&m_forward, f);
	XMStoreFloat3(&m_upVecForFlyCam, upRes);


	float moveLeftRight = 0.0f;
	float moveForwardBackward = 0.0f;

	// use input determine the values
	// A D for left & right
	// W S for forward and back

	// GetAsyncKeyState()

	// 0x8000 for key currently down
	// 0x0001 for just been pressed

	if (GetAsyncKeyState('A') & 0x8000)
	{
		moveLeftRight -= moveSpeed;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		moveLeftRight += moveSpeed;
	}

	if (GetAsyncKeyState('W') & 0x8000)
	{
		moveForwardBackward += moveSpeed;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		moveForwardBackward -= moveSpeed;
	}

	// for up & down (use pageUp & pageDown)
	float moveUpDown = 0.0f;
	if (GetAsyncKeyState(VK_PRIOR) & 0x8000) // VK_PRIOR = page up
	{
		moveUpDown += moveSpeed;
	}
	if (GetAsyncKeyState(VK_NEXT) & 0x8000) // VK_NEXT = page down
	{
		moveUpDown -= moveSpeed;
	}



	// just need to set the final values for m_eye, m_at & m_up
	XMVECTOR rightScaled = XMVectorScale(r, moveLeftRight);
	XMVECTOR forwardScaled = XMVectorScale(f, moveForwardBackward);
	XMVECTOR upScaled = XMVectorScale(upRes, moveUpDown);
	// for flying camera, also add the up Vector, for basic FPS camera don't


	XMFLOAT3 eyePos;
	eyePos.x = m_eyePosition.x;
	eyePos.y = m_eyePosition.y;
	eyePos.z = m_eyePosition.z;
	

	XMVECTOR pos = XMLoadFloat3(&eyePos);
	pos = XMVectorAdd(pos, rightScaled);
	pos = XMVectorAdd(pos, forwardScaled);
	pos = XMVectorAdd(pos, upScaled);
	XMStoreFloat3(&eyePos, pos);

	m_eyePosition.x = eyePos.x;
	m_eyePosition.y = eyePos.y;
	m_eyePosition.z = eyePos.z;

	// set the upVector
	XMFLOAT3 upTemp;
	XMStoreFloat3(&upTemp, upRes);
	
	m_upVector.x = upTemp.x;
	m_upVector.y = upTemp.y;
	m_upVector.z = upTemp.z;

	// finally the at position, eyePos + target vec
	XMVECTOR finalAt = XMVectorAdd(pos, targetVec);
	XMFLOAT3 finalAtPoint;
	XMStoreFloat3(&finalAtPoint, finalAt);

	m_lookAtPosition.x = finalAtPoint.x;
	m_lookAtPosition.y = finalAtPoint.y;
	m_lookAtPosition.z = finalAtPoint.z;

	// DONE
	if (GetAsyncKeyState('R') & 0x8000) // R for Rest
	{
		m_eyePosition.x = 0.0f;
		m_eyePosition.y = 0.0f;
		m_eyePosition.z = -5.0f;

		m_upVector.x = 0.0f;
		m_upVector.y = 1.0f;
		m_upVector.z = 0.0f;

		m_lookAtPosition.x = 0.0f;
		m_lookAtPosition.y = 0.0f;
		m_lookAtPosition.z = 0.0f;

	}

}