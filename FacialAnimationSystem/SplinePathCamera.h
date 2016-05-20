#ifndef _SPLINE_PATH_CAMERA_H_
#define _SPLINE_PATH_CAMERA_H_

#include "UserControledCamera.h"
#include "MathTypes.h"

#include <vector>


class SplinePathCamera : public UserControledCamera
{
public:
	SplinePathCamera()
	{
		m_splinePosition = 0.0f;
		m_moveSpeed = 0.01f;
	}

	virtual void shutdown();
	virtual void update(float dt);

	void setControlPoints(std::vector<Vector3D> controlPoints);
	std::vector<Vector3D> getControlPoints()
	{
		return m_controlPoints;
	}
private:
	std::vector<Vector3D> m_controlPoints;
	float m_splinePosition;

	Vector3D calculatePositionOnSpline(std::vector<Vector3D> & ctrlPts);
};


#endif