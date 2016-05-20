#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <string>
#include <chrono> // for the time between frames

#include "playBackManager.h"
#include "ErrorLog.h"

#include "Renderer.h"
#include "CameraManager.h"

#include "PhonemeMap.h"
#include "VisemeMap.h"
#include "PhonemeToVisemeMap.h"

#include "Mesh.h"

#include "AudioAnalyser.h"
#include "DialoguePattern.h"

#include "VisemeAnimatedModel.h"

class Application
{
public:
	Application();

	~Application();

	bool init(std::string indexFileName, HINSTANCE instanceHandle, int nCmdShow);

	void run();

	void shutdown();



private:
	void update(float dtSec);
	void draw();

	bool m_wantToRun;
	ErrorLog * m_errLog;

	Renderer * m_renderer;
	CameraManager * m_cameraManager;
	PhonemeMap * m_phonemeMap;
	VisemeMap * m_visemeMap;
	PhonemeToVisemeMap * m_phonemeToVisemeMap;

	DialoguePattern * m_dialogue;

	VisemeAnimatedModel * m_visemeAnimatedModel;


	PlayBackManager * m_playbackManagerPtr;
	FMOD::Sound * m_dialogueToPlay;


	Mesh testMesh;

	AudioAnalyser m_audioAnalyser;

	std::chrono::time_point<std::chrono::system_clock> m_timeAtFrameStart;
	std::chrono::time_point<std::chrono::system_clock> m_timeAtFrameEnd;
	float m_secondsSinceLastUpdate;

};


#endif