#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "MathTypes.h"

// can implerment the Cameras from "3D Computer Graphics" model
// rember Right Hand cart, NOT left!

class Camera
{
public:
	Camera()
	{
		m_eyePosition.x = 0.0f;
		m_eyePosition.y = 0.0f;
		m_eyePosition.z = 0.0f;

		m_upVector.x = 0.0f;
		m_upVector.y = 1.0f;
		m_upVector.z = 0.0f;

		m_lookAtPosition.x = 0.0f;
		m_lookAtPosition.y = 0.0f;
		m_lookAtPosition.z = -10.0f;
	}
	~Camera()
	{

	}

	Vector3D getEyePosition()
	{
		return m_eyePosition;
	}
	void setEyePosition(Vector3D eyePosition)
	{
		m_eyePosition.x = eyePosition.x;
		m_eyePosition.y = eyePosition.y;
		m_eyePosition.z = eyePosition.z;
	}

	Vector3D getUpVector()
	{
		return m_upVector;
	}
	void setUpVector(Vector3D upVec)
	{
		m_upVector.x = upVec.x;
		m_upVector.y = upVec.y;
		m_upVector.z = upVec.z;
	}

	Vector3D getLookAtPosition()
	{
		return m_lookAtPosition;
	}
	void setLookAtPosition(Vector3D lookAtPosition)
	{
		m_lookAtPosition.x = lookAtPosition.x;
		m_lookAtPosition.y = lookAtPosition.y;
		m_lookAtPosition.z = lookAtPosition.z;
	}

	virtual void shutdown(){}
	virtual void update(float dtSec){}


protected:
	Vector3D m_eyePosition;
	Vector3D m_upVector;
	Vector3D m_lookAtPosition;
};


#endif