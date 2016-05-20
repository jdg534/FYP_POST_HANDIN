#include "SplinePathCamera.h"

#include "Math.h" // my Maths Library

void SplinePathCamera::shutdown()
{

}

void SplinePathCamera::update(float dt)
{
	m_splinePosition += m_moveSpeed * dt;

	if (m_splinePosition > 1.0f)
	{
		m_splinePosition -= 1.0f;
	}

	m_eyePosition = calculatePositionOnSpline(m_controlPoints);
}

void SplinePathCamera::setControlPoints(std::vector<Vector3D> controlPoints)
{
	m_controlPoints.clear();
	for (int i = 0; i < controlPoints.size(); i++)
	{
		m_controlPoints.push_back(controlPoints[i]);
	}
}

Vector3D SplinePathCamera::calculatePositionOnSpline(std::vector<Vector3D> & ctrlPts)
{
	// will have to scale the t value to power on N points to lerp between
	int nElements = ctrlPts.size();

	if (nElements == 2) // can't get any smaller
	{
		Vector3D rv;
		rv = Math::interpolation::lerp(ctrlPts[0], ctrlPts[1], m_splinePosition);
		return rv;
	}
	else
	{
		// create next set of control points
		std::vector<Vector3D> nxtGenCtrlPts;
		for (int i = 0; i < nElements - 1; i++)
		{
			Vector3D tmpPt;
			tmpPt = Math::interpolation::lerp(ctrlPts[i], ctrlPts[i + 1], m_splinePosition);
			nxtGenCtrlPts.push_back(tmpPt);
		}
		return calculatePositionOnSpline(nxtGenCtrlPts);
	}
}