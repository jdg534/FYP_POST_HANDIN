#ifndef _FLYING_CAMERA_H_
#define _FLYING_CAMERA_H_

#include "Camera.h"

#include <DirectXMath.h>

class FlyingCamera : public Camera
{
public:

	FlyingCamera()
	{
		m_moveSpeed = 5.0f;
		m_pitch = 0.0f;
		m_yaw = 0.0f;

		m_defaultForward = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		m_defaultRight = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	}

	virtual void shutdown();
	virtual void update(float dtSec);
private:
	DirectX::XMFLOAT3 m_defaultForward;
	DirectX::XMFLOAT3 m_defaultRight;


	DirectX::XMFLOAT3 m_right;
	DirectX::XMFLOAT3 m_forward;
	DirectX::XMFLOAT3 m_upVecForFlyCam;

	DirectX::XMFLOAT3 m_lookAtDir;

	float m_moveSpeed;
	float m_pitch;
	float m_yaw;


};


#endif
