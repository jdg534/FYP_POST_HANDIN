#ifndef _CAMERA_MANAGER_H_
#define _CAMERA_MANAGER_H_

#include <vector>

#include "Camera.h"

class CameraManager
{
public:
	CameraManager();
	~CameraManager();

	bool loadCameras(std::string camDefFile);

	void shutdown();
	void updateAllCameras(float dt);
	void updateActiveCamera(float dt);

	Camera * getActiveCamera()
	{
		return m_cameras[m_activeCamera];
	}

	


private:
	

	std::vector<Camera *> m_cameras;

	unsigned int m_activeCamera;
	float m_secondsSinceLastCameraChange;
	float m_secondsToWaitBeforeCanChangeCamera;

	void changeCamera();
};


#endif