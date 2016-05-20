#include "CameraManager.h"
#include "MathTypes.h"

#include "Camera.h"
#include "UserControledCamera.h"
#include "SplinePathCamera.h"
#include "FlyingCamera.h"

#include <fstream>
#include <string>

#include <Windows.h> // keyboard input, OS specific

CameraManager::CameraManager()
{

}

CameraManager::~CameraManager()
{

}

bool CameraManager::loadCameras(std::string camDefFile)
{
	std::ifstream camsFile(camDefFile);



	// float windowWidth = (float)wPtr->getWindowWidth();
	// float windowHeight = (float)wPtr->getWindowHeight();

	//float nearDepth = 0.1f;
	//float farDepth = 100.0f; // these are to be overridden

	/*Camera * defaultCam = new Camera();
	defaultCam->setWindowWidth(windowWidth);
	defaultCam->setWindowHeight(windowHeight);

	defaultCam->setEyePosition(DirectX::XMFLOAT4(0.0f, 0.0f, -3.0f, 0.0f));
	defaultCam->setLookAtPosition(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	defaultCam->setUpVector(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f));

	m_cameras.push_back(defaultCam);
	m_activeCamera = 0;
	*/

	if (!camsFile.good())
	{
		return false;
	}

	std::string currentInstruction = "";

	while (currentInstruction != "END_OF_FILE")
	{
		camsFile >> currentInstruction;

		
		if (currentInstruction == "static_camera")
		{
			Vector3D eye, at, up; // file stores values in this order
			camsFile >> eye.x;
			camsFile >> eye.y;
			camsFile >> eye.z;
			

			camsFile >> at.x;
			camsFile >> at.y;
			camsFile >> at.z;
			

			camsFile >> up.x;
			camsFile >> up.y;
			camsFile >> up.z;
			

			Camera * tmpC = new Camera();
			//tmpC->setWindowWidth(windowWidth);
			//tmpC->setWindowHeight(windowHeight);
			tmpC->setEyePosition(eye);
			tmpC->setLookAtPosition(at);
			tmpC->setUpVector(up);

			m_cameras.push_back(tmpC);
		}
		else if (currentInstruction == "user_controled_camera")
		{
			Vector3D eye, at, up; // file stores values in this order
			camsFile >> eye.x;
			camsFile >> eye.y;
			camsFile >> eye.z;
			

			camsFile >> at.x;
			camsFile >> at.y;
			camsFile >> at.z;
			

			camsFile >> up.x;
			camsFile >> up.y;
			camsFile >> up.z;
			

			float camMoveSpeed;
			camsFile >> camMoveSpeed;

			UserControledCamera * tmpC = new UserControledCamera();
			// tmpC->setWindowWidth(windowWidth);
			// tmpC->setWindowHeight(windowHeight);
			tmpC->setEyePosition(eye);
			tmpC->setLookAtPosition(at);
			tmpC->setUpVector(up);

			tmpC->setMoveSpeed(camMoveSpeed);
			m_cameras.push_back(tmpC);
		}
		else if (currentInstruction == "spline_path_camera")
		{
			Vector3D eye, at, up; // file stores values in this order
			camsFile >> eye.x;
			camsFile >> eye.y;
			camsFile >> eye.z;
			

			camsFile >> at.x;
			camsFile >> at.y;
			camsFile >> at.z;


			camsFile >> up.x;
			camsFile >> up.y;
			camsFile >> up.z;


			// values are just initial values, will override on update anyway
			float moveSpd;
			camsFile >> moveSpd;
			int nConrolPoints;
			camsFile >> nConrolPoints;
			std::vector<Vector3D> controlPoints;
			for (int i = 0; i < nConrolPoints; i++)
			{
				Vector3D tmpCP;
				camsFile >> tmpCP.x;
				camsFile >> tmpCP.y;
				camsFile >> tmpCP.z;
				controlPoints.push_back(tmpCP);
			}
			SplinePathCamera * spc = new SplinePathCamera();
			//spc->setWindowWidth(windowWidth);
			//spc->setWindowHeight(windowHeight);
			spc->setEyePosition(eye);
			spc->setLookAtPosition(at);
			spc->setUpVector(up);

			spc->setMoveSpeed(moveSpd);

			spc->setControlPoints(controlPoints);
			m_cameras.push_back(spc);
		}
		else if (currentInstruction == "flying_camera")
		{
			FlyingCamera * fc = new FlyingCamera();
			Vector3D eye, at, up;

			camsFile >> eye.x >> eye.y >> eye.z;
			camsFile >> at.x >> at.y >> at.z;
			camsFile >> up.x >> up.y >> up.z;

			fc->setEyePosition(eye);
			fc->setLookAtPosition(at);
			fc->setUpVector(up);
			m_cameras.push_back(fc);
		}
		else if (currentInstruction == "change_camera_delay")
		{
			camsFile >> m_secondsToWaitBeforeCanChangeCamera;
		}

	}

	camsFile.close();

	/*
	add to the end once got all camera defs

	Cam->setNearDepth(0.01f);
	Cam->setFarDepth(100.0f);


	Cam->calculateViewMatrix();

	Cam->calculateProjectionMatrix();

	*/
	

	m_activeCamera = 0;
	m_secondsSinceLastCameraChange = 0.0f;
	return true;
}

void CameraManager::shutdown()
{
	for (int i = 0; i < m_cameras.size(); i++)
	{
		m_cameras[i]->shutdown();
		delete m_cameras[i];
	}
	m_cameras.clear();
}

void CameraManager::updateAllCameras(float dt)
{

	for (int i = 0; i < m_cameras.size(); i++)
	{
		m_cameras[i]->update(dt);
	}

	m_secondsSinceLastCameraChange += dt;

	if (GetAsyncKeyState(VK_TAB) & 0x8000)
	{
		changeCamera(); // only make the change if enough time has passed
	}
}

void CameraManager::updateActiveCamera(float dt)
{
	m_cameras[m_activeCamera]->update(dt);

	m_secondsSinceLastCameraChange += dt;

	if (GetAsyncKeyState(VK_TAB) & 0x8000)
	{
		changeCamera();
	}
}

void CameraManager::changeCamera()
{
	if (m_secondsSinceLastCameraChange > m_secondsToWaitBeforeCanChangeCamera)
	{
		// make the change
		m_activeCamera++;
		if (m_activeCamera == m_cameras.size())
		{
			m_activeCamera = 0;
		}
		m_secondsSinceLastCameraChange = 0.0f;
	}
}