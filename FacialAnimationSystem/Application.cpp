#include "Application.h"

#include <fstream>
#include <iostream>

#include "DialoguePattern.h" // for testing the system
#include "AudioAnalyser.h"

#include "GlobalMessageQueue.h"


Application::Application()
{

}

Application::~Application()
{
	delete m_renderer;
}

bool Application::init(std::string indexFileName, HINSTANCE instanceHandle, int nCmdShow)
{
	
	//testMesh.makeIntoTestCubeSolid();



	std::string dialogueFile = "";
	std::string animationSequence = "";

	std::ifstream indexFile(indexFileName);

	m_errLog = ErrorLog::getInstance();

	if (!indexFile.good())
	{
		m_errLog->logMinorError("Couldn't open the index file.");
		return false;
	}
	std::cout << "Configuring using " << indexFileName << std::endl;

	unsigned int silenceThreshold = 30;

	std::string currentBit = "";
	while (currentBit != "END_APP_INDEX")
	{
		indexFile >> currentBit;
		if (currentBit == "rendererFile:")
		{
			std::string renderFileName;
			std::getline(indexFile, renderFileName);

			// remove the \" (one at each end of the string)
			int firstQuote = renderFileName.find_first_of("\"");
			int lastQuote = renderFileName.find_last_of("\"");

			renderFileName = renderFileName.substr(firstQuote + 1, lastQuote - (firstQuote + 1));

			std::cout << "Creating the renderer using: " << renderFileName << std::endl;
			// create and init the renderer
			m_renderer = new Renderer();

			bool rendererSuccessfullyInitialised = m_renderer->init(renderFileName, instanceHandle, nCmdShow);
			if (!rendererSuccessfullyInitialised)
			{
				m_errLog->logFatalError("Renderer wasn't initialised successfully");
			}
		}
		else if (currentBit == "camerasFile:")
		{
			// m_errLog->logMinorError("camera manager not yet implermented");

			std::string camsFileStr;
			
			std::getline(indexFile, camsFileStr);

			// remove the \" (one at each end of the string)
			int firstQuote = camsFileStr.find_first_of("\"");
			int lastQuote = camsFileStr.find_last_of("\"");

			camsFileStr = camsFileStr.substr(firstQuote + 1, lastQuote - (firstQuote + 1));

			m_cameraManager = new CameraManager();
			std::cout << "Loading Cameras from: " << camsFileStr << std::endl;

			

			if (!m_cameraManager->loadCameras(camsFileStr))
			{
				m_errLog->logFatalError("Couldn't load the cameras from file!");
			}

		}
		else if (currentBit == "phonemes:")
		{
			// m_errLog->logMinorError("phoneme manager not yet implermented");

			// phoneme map
			m_phonemeMap = new PhonemeMap();

			std::string lineStr;
			std::getline(indexFile, lineStr);

			std::vector<int> stringQuotes;
			for (int i = 0; i < lineStr.size(); i++)
			{
				if (lineStr[i] == '"')
				{
					stringQuotes.push_back(i);
				}
			}
			std::string folderStr, fileStr;
			folderStr = lineStr.substr(stringQuotes[0] + 1, stringQuotes[1] - (stringQuotes[0] + 1));
			fileStr = lineStr.substr(stringQuotes[2] + 1, stringQuotes[3] - (stringQuotes[2] + 1));
			if (!m_phonemeMap->init(folderStr, fileStr))
			{
				m_errLog->logFatalError("Couldn't Load The Phoneme map!");
			}

		}
		else if (currentBit == "visemes:")
		{
			// m_errLog->logMinorError("viseme manager not yet implermented");

			std::string vmFile = "";
			std::getline(indexFile, vmFile);
			int q1, q2;
			q1 = vmFile.find_first_of("\"");
			q2 = vmFile.find_last_of("\"");

			vmFile = vmFile.substr(q1 + 1, q2 - 2);

			m_visemeMap = new VisemeMap();
			if (!m_visemeMap->initFromFile(vmFile))
			{
				// couldn't load the visemes
				m_errLog->logFatalError("Couldn't init the Viseme Map");
			}

		}
		else if (currentBit == "phonemeToVisemeMap:")
		{
			// m_errLog->logMinorError("phoneme to viseme mapper not yet implermented");

			std::string ptvmFileStr;
			std::getline(indexFile, ptvmFileStr);

			// remove the quotes
			m_phonemeToVisemeMap = PhonemeToVisemeMap::getInstance();
			int q1, q2;
			q1 = ptvmFileStr.find_first_of("\"");
			q2 = ptvmFileStr.find_last_of("\"");

			ptvmFileStr = ptvmFileStr.substr(q1 + 1, q2 - 2);

			
			if (!m_phonemeToVisemeMap->initFromFile(ptvmFileStr))
			{
				// couldn't load the visemes
				m_errLog->logFatalError("Couldn't init the Phoneme To Viseme Map");
			}
		}
		else if (currentBit == "dialogue:")
		{
			
			std::getline(indexFile, dialogueFile);


			// dialogueFile, stores the string for the file with the actual dialogue

			
			// "relitive path and file name"

			// remove the quotes
			int q1, q2;
			q1 = dialogueFile.find_first_of("\"");
			q2 = dialogueFile.find_last_of("\"");
			dialogueFile = dialogueFile.substr(q1 + 1, q2 - 2);

		}
		else if (currentBit == "animationSequence:")
		{
			// if it's a text file load the sequence from file
			std::getline(indexFile, animationSequence);

			// remove the quotes
			int q1, q2;
			q1 = animationSequence.find_first_of("\"");
			q2 = animationSequence.find_last_of("\"");
			animationSequence = animationSequence.substr(q1 + 1, q2 - 2);
		}
		else if (currentBit == "silenceThreshold:")
		{
			indexFile >> silenceThreshold;
		}
	}
	// deal with the dialogue patten here
	
	m_dialogue = new DialoguePattern();


	if (!m_dialogue->loadFromFile(dialogueFile))
	{
		m_errLog->logFatalError("Error couldn't load " + dialogueFile);
	}

	if (animationSequence == "USE_PATTERN_MATCHING")
	{
		AudioAnalyser aa;

		aa.setPhonemeMap(m_phonemeMap);
		aa.setSilenceThreshold(silenceThreshold);
		aa.analyseAudio(m_dialogue);
	}
	else if (animationSequence.find(".txt") != std::string::npos)
	{
		// it's a dialogue file
		if (!m_dialogue->loadStatesFromFile(animationSequence))
		{
			m_errLog->logFatalError("Couldn't load the dialogue animations from " + animationSequence);
		}
		
	}

	ID3D11Device * d3dDevicePtr = m_renderer->getD3DDevicePtr();

	// initialise the animated model using the dialogue pattern
	m_visemeAnimatedModel = new VisemeAnimatedModel();

	if (!m_visemeAnimatedModel->init(d3dDevicePtr, m_dialogue, m_visemeMap, m_phonemeToVisemeMap))
	{

	}
	
	//using namespace DirectX;
	//XMMATRIX transMat = XMMatrixTranslation(0.0f, -2.0f, 0.0f);

	// XMFLOAT4X4 matToStore;
	//XMStoreFloat4x4(&matToStore, transMat);
	//m_visemeAnimatedModel->setWorldMatrix(matToStore);


	// init code for stuff not loaded from file

	m_playbackManagerPtr = PlayBackManager::getInstance();
	m_playbackManagerPtr->init();

	// have a test dialog file loaded

	FMOD::System * soundSysPtr = m_playbackManagerPtr->getSoundSystemPtr();
	soundSysPtr->createSound(dialogueFile.c_str(), FMOD_DEFAULT, 0, &m_dialogueToPlay);

	m_playbackManagerPtr->playSound(m_dialogueToPlay, true);

	m_wantToRun = true;
	



	
	return true;
}

void Application::run()
{
	using namespace std::chrono;
	while (m_wantToRun)
	{
		m_timeAtFrameStart = system_clock::now();
		update(m_secondsSinceLastUpdate);
		draw();

		m_renderer->checkWindow();

		GlobalMessageQueue * gmq = GlobalMessageQueue::getInstance();

		unsigned int gmqMessages = gmq->getNumMessages();

		for (auto i = 0; i < gmqMessages; i++)
		{
			unsigned short msg = gmq->popMessage();
			if (msg == MT_QUIT)
			{
				m_wantToRun = false;
			}
		}
	
		/*
		if (m_renderer->isWindowOpen())
		{
			m_wantToRun = true;
		}
		else
		{
			m_wantToRun = false;
		}
		*/

		m_timeAtFrameEnd = system_clock::now();

		system_clock::duration procTime = m_timeAtFrameEnd - m_timeAtFrameStart;

		std::chrono::milliseconds procTimeInMS = duration_cast<milliseconds>(procTime);
		int ptInMsIntForm = procTimeInMS.count();

		// still in MS
		m_secondsSinceLastUpdate = static_cast<float>(ptInMsIntForm);

		// convet to seconds
		m_secondsSinceLastUpdate *= (1.0f / 1000.0f);

	}
}

void Application::shutdown()
{
	// call the shutdown function on all of the associated objects.
	//  DON'T delete them! that's done in ~Application()
	m_renderer->shutdown();
	m_cameraManager->shutdown();
	m_phonemeMap->shutdown();
	m_visemeMap->shutdown();
	m_phonemeToVisemeMap->shutdown();

	m_playbackManagerPtr->shutdown();

	m_errLog->shutdown();
	delete m_errLog;
}

void Application::update(float dtSec)
{
	// m_cameraManager->updateAllCameras(dtSec);
	m_cameraManager->updateActiveCamera(dtSec);
	m_playbackManagerPtr->update(dtSec);

	ID3D11DeviceContext * d3dDC = m_renderer->getD3DDeviceContextPtr();
	m_visemeAnimatedModel->update(dtSec, d3dDC);
}

void Application::draw()
{
	Camera * activeCam = m_cameraManager->getActiveCamera();

	m_renderer->startDrawing(activeCam); // change once got the camera class working
	// m_renderer->drawMesh(&testMesh);

	std::vector<AnimatedMesh> animatedMeshes = m_visemeAnimatedModel->getAnimatedMeshes();
	DirectX::XMFLOAT4X4 visemeAnimatedModelWorldMatrix = m_visemeAnimatedModel->getWorldMatrix();

	// pass above to the render
	m_renderer->drawAnimatedMeshes(animatedMeshes, &visemeAnimatedModelWorldMatrix);

	m_renderer->finshDrawing();

	

}