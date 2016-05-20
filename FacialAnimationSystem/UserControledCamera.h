#ifndef _USER_CONTROLED_CAMERA_H_
#define _USER_CONTROLED_CAMERA_H_

#include "Camera.h"

class UserControledCamera : public Camera
{
public:

	UserControledCamera();
	~UserControledCamera();

	virtual void shutdown();

	virtual void update(float dt);

	void setMoveSpeed(float moveSpeed)
	{
		m_moveSpeed = moveSpeed;
	}

	float getMoveSpeed()
	{
		return m_moveSpeed;
	}


protected:
	float m_moveSpeed;
};


#endif